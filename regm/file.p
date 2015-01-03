; these are comments

fn res.file.absent
    fs.stat %a
    jz +1
    retv 0

    fs.file? %a
    jz rm

    fs.symlink? %a
    jz rm

    err "%s exists, but is not a regular file"
    bail

  rm:
    fs.unlink %a
    jnz +2
    mark
    retv 0

    perror "failed to remove %s"
    bail

fn res.file.present
    fs.stat %a
    jnz create

    fs.symlink? %a
    jz remove

    err "%s exists, but is not a regular file"
    bail

  remove:
    fs.unlink %a
    jz create

    perror "failed to replace %s with a regular file"
    bail

  create:
    fs.touch %a
    jnz +2
    mark
    retv 0

    perror "failed to create regular file %s"
    bail

fn res.file.chown
    getuid %b %c
    jz ok

    ; re-arrange registers for err call
    set %a %b       ; %a is now the username, too
    err "failed to find user %s"
    bail

  ok:
    fs.chown %a %c
    jnz +2
    mark
    retv 0

    perror "failed to change ownership of %s to %s"
    bail

fn res.file.chgrp
    getgid %b %c
    jz ok

    ; re-arrange registers for err call
    set %a %b       ; %a is now the group name, too
    err "failed to find group %s"
    bail

  ok:
    fs.chgrp %a %c
    jnz +2
    mark
    retv 0

    perror "failed to change group ownership of %s to %s: %s"
    bail

fn res.file.chmod
    fs.chmod %a %b
    jnz +2
    mark
    retv 0

    perror "failed to set mode of %s to %04o: %s"
    bail

fn res.file.diff
    ; %a is path
    ; %b is remote sha1

    fs.sha1 %a %p
    jz +2
    perror "failed to calculate SHA1 for local copy of %s"
    bail

    strcmp %b %p
    ret

fn res.file.update
    ; %a is path
    ; %b is remote sha1
    ; %c is cached/not

    getfile %b %a
    jnz +1
    ret

    err "failed to update contents of %s"
    bail

fn res.file.verify
    ; %a is temp path
    ; %b is real path
    ; %c is verify command
    ; %d is expected rc

    push %b
    push %a

    set %a 0 ; run as user root
    set %b 0 ; run as group root

    exec %c %e
    cmp %d %e
    jz +1
    ret

    pop %a
    pop %b
    fs.rename %a %b
    jnz +1
    retv 0

    fs.unlink %a
    perror "failed to rename %s to %s"
    bail

fn res.file.contents
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
    retv 0

fn main
    @@@ file:/path/to/delete
      set %a "/path/to/delete"
      call res.file.absent

    @@@ file:/etc/sudoers
      set %a "/etc/sudoers"
      call res.file.present

      ;;udbopen
        set %b "root"
        call res.file.chown
        set %b "root"
        call res.file.chgrp
      ;;udbclose

      set %b 0755
      call res.file.chmod

      set %b "decafbadcafe"   ; rsha1
      set %c 1                ; cached
      set %d 1                ; do verify
      set %e "/bin/test-it -c /tmp/file"
      set %f 0
      set %g "/tmp/file"
      call res.file.contents
