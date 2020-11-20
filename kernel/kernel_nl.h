#ifndef __NETLINK_KERNEL_H__
#define __NETLINK_KERNEL_H__

#ifndef KERNEL_MOD
	#define KERNEL_MOD
#endif

#include <linux/version.h>
#include <linux/skbuff.h>
#include <net/netlink.h>
#include <net/net_namespace.h>

#include <nl.h>

typedef void (* input_handler_t)(struct sk_buff*);

static NL_RV nl_init(NetlinkCtx* _ctx, input_handler_t _rcv_fn)
{
	ASSERT_CTX_NOT_NULL(_ctx);

	memset(_ctx, 0, sizeof(NetlinkCtx));

	{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,0)
		#error "Kernel version not supported"
#elif LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0)
		_ctx->sock = netlink_kernel_create(&init_net, NETLINK_TEST_PROTO, UNICAST_MASK, _rcv_fn, NULL, THIS_MODULE);
#else
		struct netlink_kernel_cfg cfg = { .groups = UNICAST_MASK, .input = _rcv_fn };
		_ctx->sock = netlink_kernel_create(&init_net, NETLINK_TEST_PROTO, &cfg);
#endif

		if (_ctx->sock == NULL) 
			return RV_SOCKET_ERROR;
	}

	return RV_SUCCESS;
}

static NL_RV nl_free(NetlinkCtx* _ctx)
{
	ASSERT_CTX_NOT_NULL(_ctx);

	netlink_kernel_release(_ctx->sock);

	return RV_SUCCESS;
}

static NL_RV nl_send(NetlinkCtx* _ctx, __u32 _dst_port, const __u8* _buf, __u32 _len)
{
	ASSERT_CTX_NOT_NULL(_ctx);
	ASSERT_NOT_NULL(_buf);

	if (_len == 0)
		return RV_SUCCESS;

	{
		struct nlmsghdr* hdr = NULL;
		struct sk_buff* skb = NULL;
		int rv = 0;

		//Alloc
		skb = nlmsg_new(_len, 0);

		if (skb == NULL)
			return RV_BUFFER_ERROR;

		//Add header 
		//For this example, cause we use own msg types we don't really care about matching of _ctx->seq ,
		//but for more complex solutions (with different message types or for system types) you will need some synchronization mechanism
		hdr = nlmsg_put(skb, AUTO_PID, _ctx->seq, TEST_KRL_MSG, _len, NLM_F_REQUEST);

		if (hdr == NULL)
			return RV_BUFFER_ERROR;

		//Add data
		memcpy(nlmsg_data(hdr), _buf, _len);

		//Send
		rv = nlmsg_unicast(_ctx->sock, skb, _dst_port);	//will free the skb buf later

		if (rv < 0)
			return RV_SEND_ERROR;
	}

	++_ctx->seq;

	return RV_SUCCESS;
}

#endif