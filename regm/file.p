; these are comments

res.file.absent {
    call &fs.exists?
    jz +1
    ret 0

    call &fs.is_file?
    jz unlink

    call &fs.is_symlink?
    jz unlink

    err "%s exists, but is not a regular file"
    bail

  unlink:
    call &fs.unlink
    jnz +1
    ret 0

    estr %b
    err "failed to remove %s: %s"
    bail
}

res.file.present {
    call &fs.exists?
    jnz create

    call &fs.is_symlink?
    jz remove

    err "%s exists, but is not a regular file"
    bail

  remove:
    call &fs.unlink
    jz create

    estr %b
    err "failed to replace %s with a regular file: %s"
    bail

  create:
    call &fs.mkfile
    jnz +1
    ret 0

    estr %b
    err "failed to create regular file %s: %s"
    bail
}

res.file.chown {
    push %a
    set %a 1        ; find by name
                    ; %b should be username

    call &user.find
    jz ok

    ; re-arrange registers for err call
    set %a %b       ; %a is now the username, too
    err "failed to find user %s"
    bail

  ok:
    set %a %b
    call &user.get_uid

    pop %a          ; get path back
    call &fs.chown
    jnz +1
    ret 0

    estr %c
    err "failed to change ownership of %s to %s: %s"
    bail
}

res.file.chgrp {
    push %a
    set %a 1        ; find by name
                    ; %b should be group name

    call &group.find
    jz ok

    ; re-arrange registers for err call
    set %a %b       ; %a is now the group name, too
    err "failed to find group %s"
    bail

  ok:
    set %a %b
    call &group.get_gid

    pop %a          ; get path back
    call &fs.chgrp
    jnz +1
    ret 0

    estr %c
    err "failed to change group ownership of %s to %s: %s"
    bail
}

res.file.chmod
{
    call &fs.chmod
    jnz +1
    ret 0

    estr %c
    err "failed to set mode of %s to %04o: %s"
    bail
}

res.file.template
{
    ; %a is path
    ; %b is remote sha1
    ; %c is cached/not
    ; %d is verify/not
    ; %e is verify command
    ; %f is expected rc
    ; %g is tempfile

    call &fs.sha1
    jz sha1

    estr %b
    err "failed to calculate SHA1 for local copy of %s"
    bail

  sha1:
    ; local SHA1 is in %r
    strc %b %r
    jnz +1
    ret 0

    push %a
    set %a %r
    note "updating local content (%s) from remote copy (%s)"
    pop %a

    push %a
    cmp %d 0                  ; should we write to a tempfile?
    jnz update
    set %a %g                 ; set %a to a tempfile, for verification

  update:
    call &server.writefile
    jz check

    pop %a
    err "failed to update local file contents of %s"
    bail

  check:
    cmp %d 0                  ; we can ret 0 if we had no verify step
    jz verify
    ret 0

  verify;
    set %a %e
    set %b 0   ; run as user root
    set %b 0   ; run as group root
    call &exec.check
    jz +4

    estr %b
    err "failed to run command `%s`: %s"
    pop %a
    bail

    cmp %f %r
    jz rename

    set %b %r
    set %c %f
    err "pre-change verification check `%s` failed: returned %i (not %i)"
    call &fs.unlink
    pop %a
    bail

  rename:
    set %a %e
    pop %b
    call &fs.rename
    jnz +1
    ret 0

    estr %c
    err "rename of %s to %e failed: %s"
    bail
}

entry:

  set %a "/path/to/delete"
  call res.file.absent

  set %a "/etc/sudoers"
  call res.file.present

  call &users.open
    set %b "root"
    call res.file.chown
    set %b "root"
    call res.file.chgrp
  call &userdb.close

  set %b 0755
  call res.file.chmod

  set %b "decafbadcafe"   ; rsha1
  set %c 1                ; cached
  set %d 1                ; do verify
  set %e "/bin/test-it -c /tmp/file"
  set %f 0
  set %g "/tmp/file"
  call res.file.contents
