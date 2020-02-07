# The Process of Kernel Threads Switching

To get there we need to first make clear what the code does when initialize
a thread, which is mainly in `do_fork` function. Firstly, we call `alloc_proc`
to allocate memory for `struct proc` and initialize some fields. Then we set
`current` process as the parent of the new allocated one. After that we call
`setup_kstack`, `copy_mm` to allocate a kernel stack and copy `mm_struct` for
the new thread respectively. For kernel threads, `proc->cr3` should be the 
`boot_cr3`. Then we call `copy_thread` to set tf and context.
```c
// copy_thread
proc->tf = (struct trapframe *)(proc->kstack + KSTACKSIZE) - 1;
proc->context.eip = (uintptr_t)forkret;
proc->context.esp = (uintptr_t)(proc->tf);
```
In `switch_to`, we set `esp` and `eip`. After `switch_to` returns, the code goes
to `forkret`, and then call `forkrets` with `proc->tf` as arguments.
```
forkrets:
    # set stack to this new process's trapframe
    movl 4(%esp), %esp
    jmp __trapret
```
In `movl 4(%esp), %esp`, `$esp = &proc->tf`. In fact, we can make `context->eip`
point to `forkrets` directly, and comment `movl 4(%esp), %esp` this line.

After `forkrets`, the code jumps to `__trapret`. In function `kernel_thread`, we
set `tf.tf_eip` to `kernel_thread_entry`. So after `iret`, the code goes to 
`kernel_thread_entry`.
```
kernel_thread_entry:        # void kernel_thread(void)

    pushl %edx              # push arg
    call *%ebx              # call fn

    pushl %eax              # save the return value of fn(arg)
    call do_exit            # call do_exit to terminate current thread
```
```c
int
kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags) {
    ...
    tf.tf_regs.reg_ebx = (uint32_t)fn;
    tf.tf_regs.reg_edx = (uint32_t)arg;
    tf.tf_eip = (uint32_t)kernel_thread_entry;
    ...
}
```
The code of `kernel_thread_entry` is corrsponding with the function 
`kernel_thread`, using `edx` to save `arg` and `ebx` to save a pointer to the
function of `kernel_thread`.
