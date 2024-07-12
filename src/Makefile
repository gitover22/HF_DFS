CC=gcc

CPPLFAGS=-I./include \
         -I/usr/include/fastdfs \
         -I/usr/include/fastcommon \
         -I/usr/local/include/hiredis/ \
         -I/usr/include/mysql/

CFLAGS=-Wall

LIBS=-lfdfsclient \
     -lfastcommon \
     -lhiredis \
     -lfcgi \
     -lm \
     -lmysqlclient  

TEST_PATH=test
COMMON_PATH=common
CGI_BIN_PATH=bin_cgi
CGI_SRC_PATH=src_cgi

# 测试
main=main
redis=redis

# bin
login=$(CGI_BIN_PATH)/login
register=$(CGI_BIN_PATH)/register
upload=$(CGI_BIN_PATH)/upload
md5=$(CGI_BIN_PATH)/md5
myfiles=$(CGI_BIN_PATH)/myfiles
dealfile=$(CGI_BIN_PATH)/dealfile
sharefiles=$(CGI_BIN_PATH)/sharefiles
dealsharefile=$(CGI_BIN_PATH)/dealsharefile

target=$(login) \
       $(register) \
       $(upload) \
       $(md5) \
       $(myfiles) \
       $(dealfile) \
       $(sharefiles) \
       $(dealsharefile)

ALL:$(target)

$(main):
	$(CC) $(TEST_PATH)/main.c $(TEST_PATH)/fdfs_api.c $(COMMON_PATH)/make_log.c $(CPPLFAGS) $(CFLAGS) $(LIBS) -o $@

$(redis):
	$(CC) $(TEST_PATH)/myredis.c $(CPPLFAGS) $(CFLAGS) $(LIBS) -o $@

$(login) $(register) $(upload) $(md5) $(myfiles) $(dealfile) $(sharefiles) $(dealsharefile):
	$(CC) $(CGI_SRC_PATH)/$(@F:.cgi=)_cgi.c \
	$(COMMON_PATH)/make_log.c \
	$(COMMON_PATH)/cJSON.c \
	$(COMMON_PATH)/deal_mysql.c \
	$(COMMON_PATH)/redis_op.c \
	$(COMMON_PATH)/cfg.c \
	$(COMMON_PATH)/util_cgi.c \
	$(COMMON_PATH)/des.c \
	$(COMMON_PATH)/base64.c \
	$(COMMON_PATH)/md5.c \
	$(CPPLFAGS) $(CFLAGS) $(LIBS) -o $@

clean:
	-rm -f $(target) $(main) $(redis)

# 声明伪文件
.PHONY: clean ALL
