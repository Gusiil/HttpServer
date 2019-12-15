#ifndef code_HttpConnection_h
#define code_HttpConnection_h

#include <memory>

/* 负责与Channel通信，根据事件触发，自动读写Http数据到缓冲区 */
namespace webserver
{

class EventLoop;
class Channel;
class HttpHandler;

class HttpConnection
{
public:
	typedef std::shared_ptr<Channel> SP_Channel;
	enum ConnState{ kConnected=0x0, kHandle, kError, kDisConnecting, kDisconnected };
	
	HttpConnection(EventLoop *loop, int connfd);
	~HttpConnection();
	
	/* 根据Channel，自动调用 */
	void handleRead(void);
	void handleWrite(void);
	void handleClose(void);
	void handleError(void);
	
	void send(const void *data, int len);
	void send(const std::string &data);
	
	SP_Channel &getChannel() { return channel_; }
	void setDefaultCallback();
	
	/* HttpHandler独占HttpConnection，线程安全 */
	std::string getRecvBuffer() 
	{ 
		std::string buf;
		/* 上层读取完数据，__in_buffer置空 */
		std::swap(buf, __in_buffer);
		
		return buf;
	}
	
	void setHolder(std::shared_ptr<HttpHandler> handler)
	{ holder_ = handler; }
	
	/* 供HttpHandler使用 */
	ConnState getState() const { return state_; }
	void setState(ConnState state) { state_ = state; }
	void shutdown(int how);
	
private:
	EventLoop *loop_;
	int connfd_;
	SP_Channel channel_;
	std::string __in_buffer;	/* 缓冲区不宜用string，遇到'\0'就结束了 */
	std::string __out_buffer;
	
	std::weak_ptr<HttpHandler> holder_;	/* 延长HttpHandler的生命周期 */
	ConnState state_;
};

}

#endif
