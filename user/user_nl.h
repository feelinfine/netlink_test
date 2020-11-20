#ifndef __NETLINK_USER_H__
#define __NETLINK_USER_H__

#ifndef USER_APP
	#define USER_APP
#endif

#include <nl.h>

#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <cstring>

#define INVALID_SOCK_FD -1

static NL_RV nl_init(NetlinkCtx* _ctx, __u32 _timeout_sec)
{
	ASSERT_CTX_NOT_NULL(_ctx);

	memset(_ctx, 0, sizeof(NetlinkCtx));

	_ctx->sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST_PROTO);

	if (_ctx->sock == INVALID_SOCK_FD)
		return RV_SOCKET_ERROR;

	if (_timeout_sec)
	{
		//No blocking recvmsg please)
		struct timespec t;
		t.tv_sec =  _timeout_sec;
		t.tv_nsec = 0;
		setsockopt(_ctx->sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof(t));
	}

	__u32 pid =  getpid();
	_ctx->src = { AF_NETLINK, 0, pid, UNICAST_MASK };

	if (0 != bind(_ctx->sock, (struct sockaddr*)&_ctx->src, sizeof(_ctx->src)))
		return RV_SOCKET_ERROR;

	_ctx->seq = 0;

	return RV_SUCCESS;
}

static NL_RV nl_free(const NetlinkCtx* _ctx)
{
	ASSERT_CTX_NOT_NULL(_ctx)

	if (_ctx->sock != INVALID_SOCK_FD)
		close(_ctx->sock);

	return RV_SUCCESS;
}

static NL_RV nl_send(NetlinkCtx* _ctx, __u32 _dst_port, const __u8* _buf, __u32 _len)
{
	ASSERT_CTX_NOT_NULL(_ctx);
	ASSERT_NOT_NULL(_buf);

	if (_len == 0)
		return RV_SUCCESS;

	_ctx->dst = { AF_NETLINK, 0, _dst_port, UNICAST_MASK };

	static __u8 header[NLMSG_HDRLEN] = {0};	//with padding
	struct nlmsghdr* nh = (nlmsghdr*)header;

	nh->nlmsg_len = NLMSG_LENGTH(_len);		//msg length including header
	nh->nlmsg_type = TEST_USR_MSG;
	nh->nlmsg_flags = NLM_F_REQUEST;
	nh->nlmsg_seq = _ctx->seq;
	nh->nlmsg_pid = _ctx->src.nl_pid;

	iovec out[] = 
	{ 
		{ &header, sizeof(header) },
		{ (void*)_buf, _len }
	};

	msghdr msg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_name = &_ctx->dst;
	msg.msg_namelen = sizeof(_ctx->dst);
	msg.msg_iov = out;
	msg.msg_iovlen = sizeof(out)/sizeof(out[0]);

	if (-1 == sendmsg(_ctx->sock, &msg, 0))
		return RV_SEND_ERROR;

	++_ctx->seq;

	return RV_SUCCESS;
}

static NL_RV nl_recv(NetlinkCtx* _ctx, __u8* _buf, __u32* _len)
{
	ASSERT_CTX_NOT_NULL(_ctx);
	ASSERT_NOT_NULL(_buf);
	ASSERT_NOT_NULL(_len);

	//Not working for multipart messages

	static __u8 header[NLMSG_HDRLEN] = {0};	//with padding

	struct iovec in[] = 
	{ 
		{ header, sizeof(header) },
		{_buf, *_len}
	};

	struct msghdr hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.msg_name = &_ctx->dst;				//optional
	hdr.msg_namelen = sizeof(_ctx->dst);	//optional
	hdr.msg_iov = in;
	hdr.msg_iovlen = sizeof(in)/sizeof(in[0]);

	ssize_t rv = recvmsg(_ctx->sock, &hdr, 0);

	if (-1 == rv)
		return RV_RECV_ERROR;

	const struct nlmsghdr* nl_hdr = (const struct nlmsghdr*)header;

	if (nl_hdr->nlmsg_type != TEST_KRL_MSG)
		return RV_INVALID_MESSAGE;

	*_len = nl_hdr->nlmsg_len - NLMSG_HDRLEN;

	//Maybe check seq num ...

	return RV_SUCCESS;
}

#endif