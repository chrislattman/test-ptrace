#define _GNU_SOURCE // needed for kill
#include <linux/elf.h>
#include <signal.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    struct user_regs_struct regs;
    struct iovec iov;
    iov.iov_base = &regs;
    iov.iov_len = sizeof(regs);
    int data;

    pid_t pid = fork();
    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("./child", "child", (char *) 0);
    } else {
        wait(NULL);
        ptrace(PTRACE_CONT, pid, NULL, NULL);
        sleep(1);
        kill(pid, SIGINT);
        wait(NULL);
        ptrace(PTRACE_GETREGSET, pid, (void *) NT_PRSTATUS, &iov);
#if defined(__amd64__)
        printf("Register rbp = 0x%llx\n", regs.rbp);
        data = (int) ptrace(PTRACE_PEEKDATA, pid, (void *) (regs.rbp - 0x14), NULL);
        printf("arg1 = *(rbp - 0x14) = %d\n", data);
        data = (int) ptrace(PTRACE_PEEKDATA, pid, (void *) (regs.rbp - 0x18), NULL);
        printf("arg2 = *(rbp - 0x18) = %d\n", data);
        ptrace(PTRACE_POKEDATA, pid, (void *) (regs.rbp - 0x14), (void *) 100);
#elif defined(__aarch64__)
        printf("Register sp = 0x%llx\n", regs.sp);
        data = (int) ptrace(PTRACE_PEEKDATA, pid, (void *) (regs.sp + 12), NULL);
        printf("arg1 = *(sp + 12) = %d\n", data);
        data = (int) ptrace(PTRACE_PEEKDATA, pid, (void *) (regs.sp + 8), NULL);
        printf("arg2 = *(sp + 8) = %d\n", data);
        ptrace(PTRACE_POKEDATA, pid, (void *) (regs.sp + 12), (void *) 100);
#endif
        ptrace(PTRACE_CONT, pid, NULL, NULL);
        wait(NULL);
    }

    return 0;
}
