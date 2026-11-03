#ifndef _INCLUDE_LOG_H_
#define _INCLUDE_LOG_H_

typedef enum {
	LOG_CLOSE=0,
	LOG_DEBUG,
	LOG_INFOR,
	LOG_WARNI,
	LOG_ERROR,
}LOG_LEVEL;

#define LOG_MESSAGE_BUFF_SIZE 0x100
#define LOG_OUTPUT_FILTER_LEVEL LOG_DEBUG

#define JLOG(level,fmt...)	\
	jmy_log(level,__FUNCTION__,__LINE__,##fmt)

void jmy_log(const LOG_LEVEL level,const char *function,
					const int line,const char *format,...);

#endif /* _INCLUDE_LOG_H_ */
