# netlink_test
Example of communication between kernel module and user application


**Workflow:**
1. Kernel module invokes user application by calling call_usermodehelper
2. User app sends some message to kernel
3. Kernel receives message and sends response
4. User app receives kernel-response and sends something back


**First of all prepare a testing environment (maybe a virtual machine), because kernel modules may be very dangerous**

1. ``` make ```

All build artifacts (kernel module and user app) will be placed into the 'build' directory

Now insert the module

2. ``` cd build ```
3. ``` sudo insmod kernel_mod.ko ```

Show last ten log messages

4. ``` dmesg -T | tail ```

If everything's fine you will receive something like this:

```
[19:39:05 2020] User app path: /home/owl/my/cpp/test_module/build/user_app
[19:39:05 2020] Launch with option: Wake up!
[19:39:05 2020] Message from user(pid 7187) : Hello kernel, you heartless bitch!
[19:39:05 2020] Send message back: Goodbye!
[19:39:05 2020] Message from user(pid 7187) : Goodbye kernel!
```

There is an additional logfile from user app in build directory. 

Do cleanup 

5. ``` make clean ```
6. ``` sudo rmmod kernel_mod.ko ```

Tested on 5.4.0
