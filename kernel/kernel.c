#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/string.h>

#include "kernel_nl.h"

static NetlinkCtx g_ctx;

static void input_handler(struct sk_buff* _skb) 
{
	//handle request
	static int only_once = 1;

	const struct nlmsghdr *hdr = (const struct nlmsghdr*)_skb->data;
	__u32 pid = hdr->nlmsg_pid;

	printk(KERN_INFO "Message from user(pid %d) : %s\n", pid, (char*)NLMSG_DATA(hdr));

	//send response
	if (only_once)
	{
		const char* rsp = "Goodbye!";

		NL_RV rv = nl_send(&g_ctx, pid, rsp, strlen(rsp));

		if (rv)
			printk(KERN_INFO "nl_send failed: %s\n", nl_rv_str(rv));

		printk(KERN_INFO "Send message back: %s\n", rsp);

		only_once = 0;
	}
}

static int __init mod_init(void) 
{
	char user_app_path[] = STRING(BUILD_DIR)"user_app";
	char option[] = "Wake up!";

	char* av[] = {
		user_app_path, option, 
		NULL
	};

	char* env[] = {
		//"PATH=/sbin/:/usr/sbin:/bin:/usr/bin", 	//for more complex examples maybe you will need some environment
		NULL
	};

	NL_RV rv = 0;

	printk(KERN_INFO "User app path: %s\n", user_app_path);
	printk(KERN_INFO "Launch with option: %s\n", option);

	do
	{
		rv = nl_init(&g_ctx, input_handler);

		if (rv)
		{
			printk(KERN_ALERT "nl_init failed: %s\n", nl_rv_str(rv));
			break;
		}

		//Launch user app
		if (call_usermodehelper(av[0], av, env, UMH_WAIT_EXEC))	//run but no wait until finish
		{
			printk(KERN_ALERT "User app launch failed >(\n");
			break;
		}

		return 0;

	} while(0);

	//cleanup
	rv = nl_free(&g_ctx);

	if (rv)
		printk(KERN_ALERT "nl_free failed: %s\n", nl_rv_str(rv));

	return -ECANCELED;
}

static void __exit mod_exit(void) 
{
	NL_RV rv = nl_free(&g_ctx);

	if (rv)
		printk(KERN_ALERT "nl_free failed: %s\n", nl_rv_str(rv));

	printk(KERN_INFO "Goodbye, World!\n");
}

module_init(mod_init);
module_exit(mod_exit);