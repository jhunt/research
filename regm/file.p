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
    jnz +2
    mark
    ret 0

    perror "failed to remove %s"
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

    perror "failed to replace %s with a regular file"
    bail

  create:
    call &fs.mkfile
    jnz +2
    mark
    ret 0

    perror "failed to create regular file %s"
    bail
}

res.file.chown {
    getuid %b %c
    jz ok

    ; re-arrange registers for err call
    set %a %b       ; %a is now the username, too
    err "failed to find user %s"
    bail

  ok:
    chown %a %c
    jnz +2
    mark
    ret 0

    perror "failed to change ownership of %s to %s"
    bail
}

res.file.chgrp {
    getgid %b %c
    jz ok

    ; re-arrange registers for err call
    set %a %b       ; %a is now the group name, too
    err "failed to find group %s"
    bail

  ok:
    chgrp %a %c
    jnz +2
    mark
    ret 0

    perror "failed to change group ownership of %s to %s: %s"
    bail
}

res.file.chmod
{
    call &fs.chmod
    jnz +2
    mark
    ret 0

    perror "failed to set mode of %s to %04o: %s"
    bail
}

res.file.diff
{
    ; %a is path
    ; %b is remote sha1

    sha1 %a %r
    jz +2
    perror "failed to calculate SHA1 for local copy of %s"
    bail

    strcmp %b %r
    ret
}

res.file.update
{
    ; %a is path
    ; %b is remote sha1
    ; %c is cached/not

    call &server.writefile
    jnz +1
    ret

    err "failed to update contents of %s"
    bail
}

res.file.verify
{
    ; %a is temp path
    ; %b is real path
    ; %c is verify command
    ; %d is expected rc

    push %b
    push %a

    set %a %c
    set %b 0 ; run as user root
    set %c 0 ; run as group root

    call &exec %e
    cmp %d %e
    jz +1
    ret

    pop %a
    pop %b
    rename %a %b
    jnz +1
    ret 0

    unlink %a
    perror "failed to rename %s to %s"
    bail
}

res.file.contents
{
    ; %a is path
    ; %b is remote sha1
    ; %c is cached/not
    ; %d is verify/not
    ; %e is verify command
    ; %f is expected rc
    ; %g is tempfile

    call res.file.diff
    jnz +1
    ret

    ;; files differ
    cmp %d 1                  ; should we use %g (tempfile)
    jnz update                ; in place of %a (real path)?

    swap %g %a                ; yes.  update the contents of
                              ; the tempfile first, so we can
                              ; run the verify (%e) command

  update:
    call res.file.update      ; no need to check return value
                              ; res.file.update will bail if
                              ; there were any problems

    cmp %d 1                  ; run the verification?
    jz verify
    mark
    ret

  verify:
    set %b %g                 ; real path
    set %c %e                 ; verify command
    set %d %f                 ; expected rc

    call res.file.verify      ; no need to check return value
                              ; res.file.verify will bail if
                              ; there are any problems
    mark
    ret 0
}

entry:

@@@ file:/path/to/delete @@@
  set %a "/path/to/delete"
  call res.file.absent

@@@ file:/etc/sudoers @@@
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
