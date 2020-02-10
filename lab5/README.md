# 加载用户进程并运行的过程

The first process is derived from a kernel thread `kernel_thread(user_main, NULL, 0)`.
As it is a kernel thread, it has no `mm` filed (NULL) and share the same `pgdir`
with kernel. As the thread executes, it calls `kernel_execve` which trigger a
`SYSCALL_T` interrupt. The handler of the interrupt calls `do_execuve`. Then in
`do_execuve`, it first copies the name of the process and removes the current `mm`
if it exits. After that, it calls `load_icode` to load the binary code. In fact,
the `laod_icode` not only loads the code but also builds the `mm` struct. It first
creates a new `mm`, and then it call `setup_pgdir` to allocate a new physical page
as pgdir. In `setup_pgdir`, after allocating, it copies the `boot_pgdir`, which means
the new process has the same memory map as the kernel now. Back to the `load_icode`,
after creating `mm`, it copies different parts of elf using `mm_map` and `pgdir_alloc_page`.
Then it sets up the user stack with a size of four pages. To make the processor
jump to the beginning of the new program and switch the mode to user mode, we
modify `tf`. (Every time an interrupt triggered, the handler updates `current->tf`).
```
    tf->tf_cs = USER_CS;
    tf->tf_ds = tf->tf_es = tf->tf_ss = USER_DS;
    tf->tf_esp = USTACKTOP;
    tf->tf_eip = elf->e_entry;
    tf->tf_eflags |= FL_IF; // enable interrupt
```
After `laod_icode`, the syscall returns. Then when the proccessor runs `iret`, 
it jumps to the entry of the program loaded, and switch to user mode.
