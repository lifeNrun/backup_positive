/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2011 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

/* $Id: mysqlnd_structs.h 314740 2011-08-10 14:12:24Z andrey $ */

#ifndef MYSQLND_STRUCTS_H
#define MYSQLND_STRUCTS_H

#define MYSQLND_TYPEDEFED_METHODS

typedef struct st_mysqlnd_memory_pool MYSQLND_MEMORY_POOL;
typedef struct st_mysqlnd_memory_pool_chunk MYSQLND_MEMORY_POOL_CHUNK;
typedef struct st_mysqlnd_memory_pool_chunk_llist MYSQLND_MEMORY_POOL_CHUNK_LLIST;


#define MYSQLND_MEMORY_POOL_CHUNK_LIST_SIZE 100

struct st_mysqlnd_memory_pool
{
	zend_uchar *arena;
	unsigned int refcount;
	unsigned int arena_size;
	unsigned int free_size;

	MYSQLND_MEMORY_POOL_CHUNK*	(*get_chunk)(MYSQLND_MEMORY_POOL * pool, unsigned int size TSRMLS_DC);
};

struct st_mysqlnd_memory_pool_chunk
{
	size_t				app;
	MYSQLND_MEMORY_POOL	*pool;
	zend_uchar			*ptr;
	unsigned int		size;
	enum_func_status	(*resize_chunk)(MYSQLND_MEMORY_POOL_CHUNK * chunk, unsigned int size TSRMLS_DC);
	void				(*free_chunk)(MYSQLND_MEMORY_POOL_CHUNK * chunk TSRMLS_DC);
	zend_bool			from_pool;
};


typedef struct st_mysqlnd_cmd_buffer
{
	zend_uchar		*buffer;
	size_t			length;
} MYSQLND_CMD_BUFFER;


typedef struct st_mysqlnd_field
{
	const char *name;			/* Name of column */
	const char *org_name;		/* Original column name, if an alias */
	const char *table;			/* Table of column if column was a field */
	const char *org_table;		/* Org table name, if table was an alias */
	const char *db;				/* Database for table */
	const char *catalog;		/* Catalog for table */
	char *def;					/* Default value (set by mysql_list_fields) */
	unsigned long length;		/* Width of column (create length) */
	unsigned long max_length;	/* Max width for selected set */
	unsigned int name_length;
	unsigned int org_name_length;
	unsigned int table_length;
	unsigned int org_table_length;
	unsigned int db_length;
	unsigned int catalog_length;
	unsigned int def_length;
	unsigned int flags;			/* Diverse flags */
	unsigned int decimals;		/* Number of decimals in field */
	unsigned int charsetnr;		/* Character set */
	enum mysqlnd_field_types type;	/* Type of field. See mysql_com.h for types */
	char *root;
	size_t root_len;
} MYSQLND_FIELD;


typedef struct st_mysqlnd_upsert_result
{
	unsigned int	warning_count;
	unsigned int	server_status;
	uint64_t		affected_rows;
	uint64_t		last_insert_id;
} MYSQLND_UPSERT_STATUS;


typedef struct st_mysqlnd_error_info
{
	char error[MYSQLND_ERRMSG_SIZE+1];
	char sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int error_no;
} MYSQLND_ERROR_INFO;


typedef struct st_mysqlnd_infile_info
{
	php_stream	*fd;
	int			error_no;
	char		error_msg[MYSQLND_ERRMSG_SIZE + 1];
	const char	*filename;
} MYSQLND_INFILE_INFO;


/* character set information */
typedef struct st_mysqlnd_charset
{
	unsigned int	nr;
	const char		*name;
	const char		*collation;
	unsigned int	char_minlen;
	unsigned int	char_maxlen;
	const char		*comment;
	unsigned int 	(*mb_charlen)(unsigned int c);
	unsigned int 	(*mb_valid)(const char *start, const char *end);
} MYSQLND_CHARSET;


/* local infile handler */
typedef struct st_mysqlnd_infile
{
	int		(*local_infile_init)(void **ptr, char *filename, void **userdata TSRMLS_DC);
	int		(*local_infile_read)(void *ptr, char *buf, unsigned int buf_len TSRMLS_DC);
	int		(*local_infile_error)(void *ptr, char *error_msg, unsigned int error_msg_len TSRMLS_DC);
	void	(*local_infile_end)(void *ptr TSRMLS_DC);
	zval	*callback;
	void	*userdata;
} MYSQLND_INFILE;

typedef struct st_mysqlnd_options
{
	ulong		flags;

	/* init commands - we need to send them to server directly after connect */
	unsigned int	num_commands;
	char			**init_commands;

	/* configuration file information */
	char 		*cfg_file;
	char		*cfg_section;

	/*
	  We need to keep these because otherwise st_mysqlnd_conn will be changed.
	  The ABI will be broken and the methods structure will be somewhere else
	  in the memory which can crash external code. Feel free to reuse these.
	*/
	char		* unused1;
	char		* unused2;
	char		* unused3;
	char		* unused4;
	char		* unused5;

	enum_mysqlnd_protocol_type protocol;

	char 		*charset_name;
	/* maximum allowed packet size for communication */
	ulong		max_allowed_packet;

	zend_bool	numeric_and_datetime_as_unicode;
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
	zend_bool	int_and_float_native;
#endif
} MYSQLND_OPTIONS;

typedef struct st_mysqlnd_net_options
{
	/* timeouts */
	unsigned int timeout_connect;
	unsigned int timeout_read;
	unsigned int timeout_write;

	unsigned int net_read_buffer_size;

	/* SSL information */
	char		*ssl_key;
	char		*ssl_cert;
	char		*ssl_ca;
	char		*ssl_capath;
	char		*ssl_cipher;
	char		*ssl_passphrase;
	zend_bool	ssl_verify_peer;
	uint64_t	flags;
} MYSQLND_NET_OPTIONS;


typedef struct st_mysqlnd_connection MYSQLND;
typedef struct st_mysqlnd_net	MYSQLND_NET;
typedef struct st_mysqlnd_protocol	MYSQLND_PROTOCOL;
typedef struct st_mysqlnd_res	MYSQLND_RES;
typedef char** 					MYSQLND_ROW_C;		/* return data as array of strings */
typedef struct st_mysqlnd_stmt_data	MYSQLND_STMT_DATA;
typedef struct st_mysqlnd_stmt	MYSQLND_STMT;
typedef unsigned int			MYSQLND_FIELD_OFFSET;

typedef struct st_mysqlnd_param_bind MYSQLND_PARAM_BIND;

typedef struct st_mysqlnd_result_bind MYSQLND_RESULT_BIND;

typedef struct st_mysqlnd_result_metadata MYSQLND_RES_METADATA;
typedef struct st_mysqlnd_buffered_result MYSQLND_RES_BUFFERED;
typedef struct st_mysqlnd_unbuffered_result MYSQLND_RES_UNBUFFERED;

typedef struct st_mysqlnd_debug MYSQLND_DEBUG;


typedef MYSQLND_RES* (*mysqlnd_stmt_use_or_store_func)(MYSQLND_STMT * const TSRMLS_DC);
typedef enum_func_status  (*mysqlnd_fetch_row_func)(MYSQLND_RES *result,
													void *param,
													unsigned int flags,
													zend_bool *fetched_anything
													TSRMLS_DC);

typedef struct st_mysqlnd_stats MYSQLND_STATS;

typedef void (*mysqlnd_stat_trigger)(MYSQLND_STATS * stats, enum_mysqlnd_collected_stats stat, int64_t change TSRMLS_DC);

struct st_mysqlnd_stats
{
	uint64_t				*values;
	mysqlnd_stat_trigger	*triggers;
	size_t					count;
	zend_bool				in_trigger;
#ifdef ZTS
	MUTEX_T	LOCK_access;
#endif
};


typedef struct st_mysqlnd_read_buffer {
	zend_uchar 	* data;
	size_t 		offset;
	size_t 		size;
	size_t		len;
	zend_bool	(*is_empty)(struct st_mysqlnd_read_buffer *);
	void		(*read)(struct st_mysqlnd_read_buffer *, size_t count, zend_uchar * dest);
	size_t		(*bytes_left)(struct st_mysqlnd_read_buffer *);
	void		(*free_buffer)(struct st_mysqlnd_read_buffer ** TSRMLS_DC);
} MYSQLND_READ_BUFFER;



typedef enum_func_status	(*func_mysqlnd_net__connect)(MYSQLND_NET * net, const char * const scheme, size_t scheme_len, zend_bool persistent, char **errstr, int * errcode TSRMLS_DC);
typedef size_t				(*func_mysqlnd_net__send)(MYSQLND * const conn, char * const buf, size_t count TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_net__receive)(MYSQLND * conn, zend_uchar * buffer, size_t count TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_net__set_client_option)(MYSQLND_NET * const net, enum_mysqlnd_option option, const char * const value TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_net__network_read)(MYSQLND * conn, zend_uchar * buffer, size_t count TSRMLS_DC);
typedef size_t				(*func_mysqlnd_net__network_write)(MYSQLND * const conn, const zend_uchar * const buf, size_t count TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_net__decode)(zend_uchar * uncompressed_data, size_t uncompressed_data_len, const zend_uchar * const compressed_data, size_t compressed_data_len TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_net__encode)(zend_uchar * compress_buffer, size_t compress_buffer_len, const zend_uchar * const uncompressed_data, size_t uncompressed_data_len TSRMLS_DC);
typedef size_t				(*func_mysqlnd_net__consume_uneaten_data)(MYSQLND_NET * const net, enum php_mysqlnd_server_command cmd TSRMLS_DC);
typedef void				(*func_mysqlnd_net__free_contents)(MYSQLND_NET * net TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_net__enable_ssl)(MYSQLND_NET * const net TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_net__disable_ssl)(MYSQLND_NET * const net TSRMLS_DC);


struct st_mysqlnd_net_methods
{
	func_mysqlnd_net__connect connect;
	func_mysqlnd_net__send send;
	func_mysqlnd_net__receive receive;
	func_mysqlnd_net__set_client_option set_client_option;
	func_mysqlnd_net__network_read network_read;
	func_mysqlnd_net__network_write network_write;
	func_mysqlnd_net__decode decode;
	func_mysqlnd_net__encode encode;
	func_mysqlnd_net__consume_uneaten_data consume_uneaten_data;
	func_mysqlnd_net__free_contents free_contents;
	func_mysqlnd_net__enable_ssl enable_ssl;
	func_mysqlnd_net__disable_ssl disable_ssl;

	void * unused1;
	void * unused2;
	void * unused3;
	void * unused4;
	void * unused5;
};


struct st_mysqlnd_packet_greet;
struct st_mysqlnd_packet_greet;
struct st_mysqlnd_packet_auth;
struct st_mysqlnd_packet_ok;
struct st_mysqlnd_packet_command;
struct st_mysqlnd_packet_eof;
struct st_mysqlnd_packet_rset_header;
struct st_mysqlnd_packet_res_field;
struct st_mysqlnd_packet_row;
struct st_mysqlnd_packet_stats;
struct st_mysqlnd_packet_prepare_response;
struct st_mysqlnd_packet_chg_user_resp;

typedef struct st_mysqlnd_packet_greet *		(*func_mysqlnd_protocol__get_greet_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_auth *			(*func_mysqlnd_protocol__get_auth_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_ok *			(*func_mysqlnd_protocol__get_ok_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_command *		(*func_mysqlnd_protocol__get_command_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_eof *			(*func_mysqlnd_protocol__get_eof_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_rset_header *	(*func_mysqlnd_protocol__get_rset_header_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_res_field *	(*func_mysqlnd_protocol__get_result_field_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_row *			(*func_mysqlnd_protocol__get_row_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_stats *		(*func_mysqlnd_protocol__get_stats_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_prepare_response *(*func_mysqlnd_protocol__get_prepare_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);
typedef struct st_mysqlnd_packet_chg_user_resp*(*func_mysqlnd_protocol__get_change_user_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC);

struct st_mysqlnd_protocol_methods
{
	func_mysqlnd_protocol__get_greet_packet get_greet_packet;
	func_mysqlnd_protocol__get_auth_packet get_auth_packet;
	func_mysqlnd_protocol__get_ok_packet get_ok_packet;
	func_mysqlnd_protocol__get_command_packet get_command_packet;
	func_mysqlnd_protocol__get_eof_packet get_eof_packet;
	func_mysqlnd_protocol__get_rset_header_packet get_rset_header_packet;
	func_mysqlnd_protocol__get_result_field_packet get_result_field_packet;
	func_mysqlnd_protocol__get_row_packet get_row_packet;
	func_mysqlnd_protocol__get_stats_packet get_stats_packet;
	func_mysqlnd_protocol__get_prepare_response_packet get_prepare_response_packet;
	func_mysqlnd_protocol__get_change_user_response_packet get_change_user_response_packet;

	void * unused1;
	void * unused2;
	void * unused3;
	void * unused4;
	void * unused5;
};



typedef enum_func_status	(*func_mysqlnd_conn__init)(MYSQLND * conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__connect)(MYSQLND *conn, const char *host, const char * user, const char * passwd, unsigned int passwd_len, const char * db, unsigned int db_len, unsigned int port, const char * socket_or_pipe, unsigned int mysql_flags TSRMLS_DC);
typedef ulong				(*func_mysqlnd_conn__escape_string)(MYSQLND * const conn, char *newstr, const char *escapestr, size_t escapestr_len TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__set_charset)(MYSQLND * const conn, const char * const charset TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__query)(MYSQLND *conn, const char *query, unsigned int query_len TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__send_query)(MYSQLND *conn, const char *query, unsigned int query_len TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__reap_query)(MYSQLND *conn TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_conn__use_result)(MYSQLND * const conn TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_conn__store_result)(MYSQLND * const conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__next_result)(MYSQLND * const conn TSRMLS_DC);
typedef zend_bool			(*func_mysqlnd_conn__more_results)(const MYSQLND * const conn TSRMLS_DC);

typedef MYSQLND_STMT *		(*func_mysqlnd_conn__stmt_init)(MYSQLND * const conn TSRMLS_DC);

typedef enum_func_status	(*func_mysqlnd_conn__shutdown_server)(MYSQLND * const conn, uint8_t level TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__refresh_server)(MYSQLND * const conn, uint8_t options TSRMLS_DC);

typedef enum_func_status	(*func_mysqlnd_conn__ping)(MYSQLND * const conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__kill_connection)(MYSQLND *conn, unsigned int pid TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__select_db)(MYSQLND * const conn, const char * const db, unsigned int db_len TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__server_dump_debug_information)(MYSQLND * const conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__change_user)(MYSQLND * const conn, const char * user, const char * passwd, const char * db, zend_bool silent TSRMLS_DC);

typedef unsigned int		(*func_mysqlnd_conn__get_error_no)(const MYSQLND * const conn TSRMLS_DC);
typedef const char *		(*func_mysqlnd_conn__get_error_str)(const MYSQLND * const conn TSRMLS_DC);
typedef const char *		(*func_mysqlnd_conn__get_sqlstate)(const MYSQLND * const conn TSRMLS_DC);
typedef uint64_t			(*func_mysqlnd_conn__get_thread_id)(const MYSQLND * const conn TSRMLS_DC);
typedef void				(*func_mysqlnd_conn__get_statistics)(const MYSQLND * const conn, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);

typedef unsigned long		(*func_mysqlnd_conn__get_server_version)(const MYSQLND * const conn TSRMLS_DC);
typedef const char *		(*func_mysqlnd_conn__get_server_information)(const MYSQLND * const conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__get_server_statistics)(MYSQLND *conn, char **message, unsigned int * message_len TSRMLS_DC);
typedef const char *		(*func_mysqlnd_conn__get_host_information)(const MYSQLND * const conn TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_conn__get_protocol_information)(const MYSQLND * const conn TSRMLS_DC);
typedef const char *		(*func_mysqlnd_conn__get_last_message)(const MYSQLND * const conn TSRMLS_DC);
typedef const char *		(*func_mysqlnd_conn__charset_name)(const MYSQLND * const conn TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_conn__list_fields)(MYSQLND *conn, const char *table, const char *achtung_wild TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_conn__list_method)(MYSQLND *conn, const char *query, const char *achtung_wild, char *par1 TSRMLS_DC);

typedef uint64_t			(*func_mysqlnd_conn__get_last_insert_id)(const MYSQLND * const conn TSRMLS_DC);
typedef uint64_t			(*func_mysqlnd_conn__get_affected_rows)(const MYSQLND * const conn TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_conn__get_warning_count)(const MYSQLND * const conn TSRMLS_DC);

typedef unsigned int		(*func_mysqlnd_conn__get_field_count)(const MYSQLND * const conn TSRMLS_DC);

typedef enum_func_status	(*func_mysqlnd_conn__set_server_option)(MYSQLND * const conn, enum_mysqlnd_server_option option TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__set_client_option)(MYSQLND * const conn, enum_mysqlnd_option option, const char * const value TSRMLS_DC);
typedef void				(*func_mysqlnd_conn__free_contents)(MYSQLND *conn TSRMLS_DC);	/* private */
typedef void				(*func_mysqlnd_conn__free_options)(MYSQLND * conn TSRMLS_DC);	/* private */
typedef enum_func_status	(*func_mysqlnd_conn__close)(MYSQLND *conn, enum_connection_close_type close_type TSRMLS_DC);
typedef void				(*func_mysqlnd_conn__dtor)(MYSQLND *conn TSRMLS_DC);	/* private */

typedef enum_func_status	(*func_mysqlnd_conn__query_read_result_set_header)(MYSQLND *conn, MYSQLND_STMT *stmt TSRMLS_DC);

typedef MYSQLND *			(*func_mysqlnd_conn__get_reference)(MYSQLND * const conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__free_reference)(MYSQLND * const conn TSRMLS_DC);
typedef enum mysqlnd_connection_state (*func_mysqlnd_conn__get_state)(MYSQLND * const conn TSRMLS_DC);
typedef void				(*func_mysqlnd_conn__set_state)(MYSQLND * const conn, enum mysqlnd_connection_state new_state TSRMLS_DC);

typedef enum_func_status	(*func_mysqlnd_conn__simple_command)(MYSQLND *conn, enum php_mysqlnd_server_command command, const char * const arg, size_t arg_len, enum mysqlnd_packet_type ok_packet, zend_bool silent, zend_bool ignore_upsert_status TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__simple_command_handle_response)(MYSQLND *conn, enum mysqlnd_packet_type ok_packet, zend_bool silent, enum php_mysqlnd_server_command command, zend_bool ignore_upsert_status TSRMLS_DC);

typedef enum_func_status	(*func_mysqlnd_conn__restart_psession)(MYSQLND *conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__end_psession)(MYSQLND *conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__send_close)(MYSQLND * conn TSRMLS_DC);

typedef enum_func_status    (*func_mysqlnd_conn__ssl_set)(MYSQLND * const conn, const char * key, const char * const cert, const char * const ca, const char * const capath, const char * const cipher TSRMLS_DC);

typedef MYSQLND_RES * 		(*func_mysqlnd_conn__result_init)(unsigned int field_count, zend_bool persistent TSRMLS_DC);

typedef enum_func_status	(*func_mysqlnd_conn__set_autocommit)(MYSQLND * conn, unsigned int mode TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__tx_commit)(MYSQLND * conn TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_conn__tx_rollback)(MYSQLND * conn TSRMLS_DC);


struct st_mysqlnd_conn_methods
{
	func_mysqlnd_conn__init init;
	func_mysqlnd_conn__connect connect;
	func_mysqlnd_conn__escape_string escape_string;
	func_mysqlnd_conn__set_charset set_charset;
	func_mysqlnd_conn__query query;
	func_mysqlnd_conn__send_query send_query;
	func_mysqlnd_conn__reap_query reap_query;
	func_mysqlnd_conn__use_result use_result;
	func_mysqlnd_conn__store_result store_result;
	func_mysqlnd_conn__next_result next_result;
	func_mysqlnd_conn__more_results more_results;

	func_mysqlnd_conn__stmt_init stmt_init;

	func_mysqlnd_conn__shutdown_server shutdown_server;
	func_mysqlnd_conn__refresh_server refresh_server;

	func_mysqlnd_conn__ping ping;
	func_mysqlnd_conn__kill_connection kill_connection;
	func_mysqlnd_conn__select_db select_db;
	func_mysqlnd_conn__server_dump_debug_information server_dump_debug_information;
	func_mysqlnd_conn__change_user change_user;

	func_mysqlnd_conn__get_error_no get_error_no;
	func_mysqlnd_conn__get_error_str get_error_str;
	func_mysqlnd_conn__get_sqlstate get_sqlstate;
	func_mysqlnd_conn__get_thread_id get_thread_id;
	func_mysqlnd_conn__get_statistics get_statistics;

	func_mysqlnd_conn__get_server_version get_server_version;
	func_mysqlnd_conn__get_server_information get_server_information;
	func_mysqlnd_conn__get_server_statistics get_server_statistics;
	func_mysqlnd_conn__get_host_information get_host_information;
	func_mysqlnd_conn__get_protocol_information get_protocol_information;
	func_mysqlnd_conn__get_last_message get_last_message;
	func_mysqlnd_conn__charset_name charset_name;
	func_mysqlnd_conn__list_fields list_fields;
	func_mysqlnd_conn__list_method list_method;

	func_mysqlnd_conn__get_last_insert_id get_last_insert_id;
	func_mysqlnd_conn__get_affected_rows get_affected_rows;
	func_mysqlnd_conn__get_warning_count get_warning_count;

	func_mysqlnd_conn__get_field_count get_field_count;

	func_mysqlnd_conn__set_server_option set_server_option;
	func_mysqlnd_conn__set_client_option set_client_option;
	func_mysqlnd_conn__free_contents free_contents;
	func_mysqlnd_conn__free_options free_options;
	func_mysqlnd_conn__close close;
	func_mysqlnd_conn__dtor dtor;

	func_mysqlnd_conn__query_read_result_set_header query_read_result_set_header;

	func_mysqlnd_conn__get_reference get_reference;
	func_mysqlnd_conn__free_reference free_reference;
	func_mysqlnd_conn__get_state get_state;
	func_mysqlnd_conn__set_state set_state;

	func_mysqlnd_conn__simple_command simple_command;
	func_mysqlnd_conn__simple_command_handle_response simple_command_handle_response;

	func_mysqlnd_conn__restart_psession restart_psession;
	func_mysqlnd_conn__end_psession end_psession;
	func_mysqlnd_conn__send_close send_close;

	func_mysqlnd_conn__ssl_set ssl_set;

	func_mysqlnd_conn__result_init result_init;
#ifdef AUTOCOMMIT_TX_COMMIT_ROLLBACK
	func_mysqlnd_conn__set_autocommit set_autocommit;
	func_mysqlnd_conn__tx_commit tx_commit;
	func_mysqlnd_conn__tx_rollback tx_rollback;
#endif
};




typedef mysqlnd_fetch_row_func	fetch_row;
typedef mysqlnd_fetch_row_func	fetch_row_normal_buffered; /* private */
typedef mysqlnd_fetch_row_func	fetch_row_normal_unbuffered; /* private */

typedef MYSQLND_RES *		(*func_mysqlnd_res__use_result)(MYSQLND_RES * const result, zend_bool ps_protocol TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_res__store_result)(MYSQLND_RES * result, MYSQLND * const conn, zend_bool ps TSRMLS_DC);
typedef void 				(*func_mysqlnd_res__fetch_into)(MYSQLND_RES *result, unsigned int flags, zval *return_value, enum_mysqlnd_extension ext TSRMLS_DC ZEND_FILE_LINE_DC);
typedef MYSQLND_ROW_C 		(*func_mysqlnd_res__fetch_row_c)(MYSQLND_RES *result TSRMLS_DC);
typedef void 				(*func_mysqlnd_res__fetch_all)(MYSQLND_RES *result, unsigned int flags, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);
typedef void 				(*func_mysqlnd_res__fetch_field_data)(MYSQLND_RES *result, unsigned int offset, zval *return_value TSRMLS_DC);
typedef uint64_t			(*func_mysqlnd_res__num_rows)(const MYSQLND_RES * const result TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_res__num_fields)(const MYSQLND_RES * const result TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_res__skip_result)(MYSQLND_RES * const result TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_res__seek_data)(MYSQLND_RES * result, uint64_t row TSRMLS_DC);
typedef MYSQLND_FIELD_OFFSET (*func_mysqlnd_res__seek_field)(MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET field_offset TSRMLS_DC);
typedef MYSQLND_FIELD_OFFSET (*func_mysqlnd_res__field_tell)(const MYSQLND_RES * const result TSRMLS_DC);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_field)(MYSQLND_RES * const result TSRMLS_DC);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_field_direct)(MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET fieldnr TSRMLS_DC);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_fields)(MYSQLND_RES * const result TSRMLS_DC);

typedef enum_func_status	(*func_mysqlnd_res__read_result_metadata)(MYSQLND_RES *result, MYSQLND * conn TSRMLS_DC);
typedef unsigned long *		(*func_mysqlnd_res__fetch_lengths)(MYSQLND_RES * const result TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_res__store_result_fetch_data)(MYSQLND * const conn, MYSQLND_RES *result, MYSQLND_RES_METADATA *meta, zend_bool binary_protocol, zend_bool to_cache TSRMLS_DC);
typedef enum_func_status 	(*func_mysqlnd_res__initialize_result_set_rest)(MYSQLND_RES * const result TSRMLS_DC);

typedef void				(*func_mysqlnd_res__free_result_buffers)(MYSQLND_RES * result TSRMLS_DC);	/* private */
typedef enum_func_status	(*func_mysqlnd_res__free_result)(MYSQLND_RES * result, zend_bool implicit TSRMLS_DC);
typedef void				(*func_mysqlnd_res__free_result_internal)(MYSQLND_RES *result TSRMLS_DC);
typedef void				(*func_mysqlnd_res__free_result_contents)(MYSQLND_RES *result TSRMLS_DC);
typedef void				(*func_mysqlnd_res__free_buffered_data)(MYSQLND_RES *result TSRMLS_DC);
typedef void				(*func_mysqlnd_res__unbuffered_free_last_data)(MYSQLND_RES *result TSRMLS_DC);

	/* for decoding - binary or text protocol */
typedef enum_func_status	(*func_mysqlnd_res__row_decoder)(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
									unsigned int field_count, MYSQLND_FIELD *fields_metadata,
									zend_bool persistent,
									zend_bool as_unicode, zend_bool as_int_or_float,
									MYSQLND_STATS * stats TSRMLS_DC);

typedef MYSQLND_RES_METADATA * (*func_mysqlnd_res__result_meta_init)(unsigned int field_count, zend_bool persistent TSRMLS_DC);

struct st_mysqlnd_res_methods
{
	mysqlnd_fetch_row_func	fetch_row;
	mysqlnd_fetch_row_func	fetch_row_normal_buffered; /* private */
	mysqlnd_fetch_row_func	fetch_row_normal_unbuffered; /* private */

	func_mysqlnd_res__use_result use_result;
	func_mysqlnd_res__store_result store_result;
	func_mysqlnd_res__fetch_into fetch_into;
	func_mysqlnd_res__fetch_row_c fetch_row_c;
	func_mysqlnd_res__fetch_all fetch_all;
	func_mysqlnd_res__fetch_field_data fetch_field_data;
	func_mysqlnd_res__num_rows num_rows;
	func_mysqlnd_res__num_fields num_fields;
	func_mysqlnd_res__skip_result skip_result;
	func_mysqlnd_res__seek_data seek_data;
	func_mysqlnd_res__seek_field seek_field;
	func_mysqlnd_res__field_tell field_tell;
	func_mysqlnd_res__fetch_field fetch_field;
	func_mysqlnd_res__fetch_field_direct fetch_field_direct;
	func_mysqlnd_res__fetch_fields fetch_fields;
	func_mysqlnd_res__read_result_metadata read_result_metadata;
	func_mysqlnd_res__fetch_lengths fetch_lengths;
	func_mysqlnd_res__store_result_fetch_data store_result_fetch_data;
	func_mysqlnd_res__initialize_result_set_rest initialize_result_set_rest;
	func_mysqlnd_res__free_result_buffers free_result_buffers;
	func_mysqlnd_res__free_result free_result;
	func_mysqlnd_res__free_result_internal free_result_internal;
	func_mysqlnd_res__free_result_contents free_result_contents;
	func_mysqlnd_res__free_buffered_data free_buffered_data;
	func_mysqlnd_res__unbuffered_free_last_data unbuffered_free_last_data;

	/* for decoding - binary or text protocol */
	func_mysqlnd_res__row_decoder row_decoder;

	func_mysqlnd_res__result_meta_init result_meta_init;

	void * unused1;
	void * unused2;
	void * unused3;
	void * unused4;
	void * unused5;
};


typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_field)(MYSQLND_RES_METADATA * const meta TSRMLS_DC);
typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_field_direct)(const MYSQLND_RES_METADATA * const meta, MYSQLND_FIELD_OFFSET fieldnr TSRMLS_DC);
typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_fields)(MYSQLND_RES_METADATA * const meta TSRMLS_DC);
typedef MYSQLND_FIELD_OFFSET	(*func_mysqlnd_res_meta__field_tell)(const MYSQLND_RES_METADATA * const meta TSRMLS_DC);
typedef enum_func_status		(*func_mysqlnd_res_meta__read_metadata)(MYSQLND_RES_METADATA * const meta, MYSQLND * conn TSRMLS_DC);
typedef MYSQLND_RES_METADATA *	(*func_mysqlnd_res_meta__clone_metadata)(const MYSQLND_RES_METADATA * const meta, zend_bool persistent TSRMLS_DC);
typedef void					(*func_mysqlnd_res_meta__free_metadata)(MYSQLND_RES_METADATA * meta TSRMLS_DC);

struct st_mysqlnd_res_meta_methods
{
	func_mysqlnd_res_meta__fetch_field fetch_field;
	func_mysqlnd_res_meta__fetch_field_direct fetch_field_direct;
	func_mysqlnd_res_meta__fetch_fields fetch_fields;
	func_mysqlnd_res_meta__field_tell field_tell;
	func_mysqlnd_res_meta__read_metadata read_metadata;
	func_mysqlnd_res_meta__clone_metadata clone_metadata;
	func_mysqlnd_res_meta__free_metadata free_metadata;
};


typedef enum_func_status	(*func_mysqlnd_stmt__prepare)(MYSQLND_STMT * const stmt, const char * const query, unsigned int query_len TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__execute)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__use_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__store_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__get_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef zend_bool			(*func_mysqlnd_stmt__more_results)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__next_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__free_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__seek_data)(const MYSQLND_STMT * const stmt, uint64_t row TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__reset)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__net_close)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC); /* private */
typedef enum_func_status	(*func_mysqlnd_stmt__dtor)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC); /* use this for mysqlnd_stmt_close */
typedef enum_func_status	(*func_mysqlnd_stmt__fetch)(MYSQLND_STMT * const stmt, zend_bool * const fetched_anything TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_parameters)(MYSQLND_STMT * const stmt, MYSQLND_PARAM_BIND * const param_bind TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_one_parameter)(MYSQLND_STMT * const stmt, unsigned int param_no, zval * const zv, zend_uchar	type TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__refresh_bind_param)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_result)(MYSQLND_STMT * const stmt, MYSQLND_RESULT_BIND * const result_bind TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_one_result)(MYSQLND_STMT * const stmt, unsigned int param_no TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__send_long_data)(MYSQLND_STMT * const stmt, unsigned int param_num, const char * const data, unsigned long length TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__get_parameter_metadata)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__get_result_metadata)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef uint64_t			(*func_mysqlnd_stmt__get_last_insert_id)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef uint64_t			(*func_mysqlnd_stmt__get_affected_rows)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef uint64_t			(*func_mysqlnd_stmt__get_num_rows)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_stmt__get_param_count)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_stmt__get_field_count)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_stmt__get_warning_count)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_stmt__get_error_no)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef const char *		(*func_mysqlnd_stmt__get_error_str)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef const char *		(*func_mysqlnd_stmt__get_sqlstate)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__get_attribute)(const MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, void * const value TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__set_attribute)(MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, const void * const value TSRMLS_DC);
typedef MYSQLND_PARAM_BIND *(*func_mysqlnd_stmt__alloc_param_bind)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef MYSQLND_RESULT_BIND*(*func_mysqlnd_stmt__alloc_result_bind)(MYSQLND_STMT * const stmt TSRMLS_DC);
typedef	void 				(*func_mysqlnd_stmt__free_parameter_bind)(MYSQLND_STMT * const stmt, MYSQLND_PARAM_BIND * TSRMLS_DC);
typedef	void 				(*func_mysqlnd_stmt__free_result_bind)(MYSQLND_STMT * const stmt, MYSQLND_RESULT_BIND * TSRMLS_DC);
typedef unsigned int		(*func_mysqlnd_stmt__server_status)(const MYSQLND_STMT * const stmt TSRMLS_DC);
typedef enum_func_status 	(*func_mysqlnd_stmt__generate_execute_request)(MYSQLND_STMT * const s, zend_uchar ** request, size_t *request_len, zend_bool * free_buffer TSRMLS_DC);
typedef enum_func_status	(*func_mysqlnd_stmt__parse_execute_response)(MYSQLND_STMT * const s TSRMLS_DC);
typedef void 				(*func_mysqlnd_stmt__free_stmt_content)(MYSQLND_STMT * const s TSRMLS_DC);

struct st_mysqlnd_stmt_methods
{
	func_mysqlnd_stmt__prepare prepare;
	func_mysqlnd_stmt__execute execute;
	func_mysqlnd_stmt__use_result use_result;
	func_mysqlnd_stmt__store_result store_result;
	func_mysqlnd_stmt__get_result get_result;
	func_mysqlnd_stmt__more_results more_results;
	func_mysqlnd_stmt__next_result next_result;
	func_mysqlnd_stmt__free_result free_result;
	func_mysqlnd_stmt__seek_data seek_data;
	func_mysqlnd_stmt__reset reset;
	func_mysqlnd_stmt__net_close net_close;
	func_mysqlnd_stmt__dtor dtor;
	func_mysqlnd_stmt__fetch fetch;

	func_mysqlnd_stmt__bind_parameters bind_parameters;
	func_mysqlnd_stmt__bind_one_parameter bind_one_parameter;
	func_mysqlnd_stmt__refresh_bind_param refresh_bind_param;
	func_mysqlnd_stmt__bind_result bind_result;
	func_mysqlnd_stmt__bind_one_result bind_one_result;
	func_mysqlnd_stmt__send_long_data send_long_data;
	func_mysqlnd_stmt__get_parameter_metadata get_parameter_metadata;
	func_mysqlnd_stmt__get_result_metadata get_result_metadata;

	func_mysqlnd_stmt__get_last_insert_id get_last_insert_id;
	func_mysqlnd_stmt__get_affected_rows get_affected_rows;
	func_mysqlnd_stmt__get_num_rows get_num_rows;

	func_mysqlnd_stmt__get_param_count get_param_count;
	func_mysqlnd_stmt__get_field_count get_field_count;
	func_mysqlnd_stmt__get_warning_count get_warning_count;

	func_mysqlnd_stmt__get_error_no get_error_no;
	func_mysqlnd_stmt__get_error_str get_error_str;
	func_mysqlnd_stmt__get_sqlstate get_sqlstate;

	func_mysqlnd_stmt__get_attribute get_attribute;
	func_mysqlnd_stmt__set_attribute set_attribute;

	func_mysqlnd_stmt__alloc_param_bind alloc_parameter_bind;
	func_mysqlnd_stmt__alloc_result_bind alloc_result_bind;

	func_mysqlnd_stmt__free_parameter_bind free_parameter_bind;
	func_mysqlnd_stmt__free_result_bind free_result_bind;

	func_mysqlnd_stmt__server_status get_server_status;

	func_mysqlnd_stmt__generate_execute_request generate_execute_request;
	func_mysqlnd_stmt__parse_execute_response parse_execute_response;

	func_mysqlnd_stmt__free_stmt_content free_stmt_content;
};


struct st_mysqlnd_net
{
	php_stream			*stream;
	struct st_mysqlnd_net_methods m;

	/* sequence for simple checking of correct packets */
	zend_uchar			packet_no;
	zend_bool			compressed;
	zend_uchar			compressed_envelope_packet_no;
#ifdef MYSQLND_COMPRESSION_ENABLED
	MYSQLND_READ_BUFFER	* uncompressed_data;
#endif
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	zend_uchar			last_command;
#endif
	/* cmd buffer */
	MYSQLND_CMD_BUFFER	cmd_buffer;

	MYSQLND_NET_OPTIONS	options;

	zend_bool			persistent;
};


struct st_mysqlnd_protocol
{
	struct st_mysqlnd_protocol_methods m;
	zend_bool persistent;
};


struct st_mysqlnd_connection
{
/* Operation related */
	MYSQLND_NET		* net;
	MYSQLND_PROTOCOL * protocol;

/* Information related */
	char			*host;
	unsigned int	host_len;
	char			*unix_socket;
	unsigned int	unix_socket_len;
	char			*user;
	unsigned int	user_len;
	char			*passwd;
	unsigned int	passwd_len;
	char			*scheme;
	unsigned int	scheme_len;
	uint64_t		thread_id;
	char			*server_version;
	char			*host_info;
	unsigned char	*scramble;
	const MYSQLND_CHARSET *charset;
	const MYSQLND_CHARSET *greet_charset;
	char			*connect_or_select_db;
	unsigned int	connect_or_select_db_len;
	MYSQLND_INFILE	infile;
	unsigned int	protocol_version;
	unsigned long	max_packet_size;
	unsigned int	port;
	unsigned long	client_flag;
	unsigned long	server_capabilities;

	/* For UPSERT queries */
	MYSQLND_UPSERT_STATUS upsert_status;
	char			*last_message;
	unsigned int	last_message_len;

	/* If error packet, we use these */
	MYSQLND_ERROR_INFO	error_info;

	/*
	  To prevent queries during unbuffered fetches. Also to
	  mark the connection as destroyed for garbage collection.
	*/
	enum mysqlnd_connection_state	state;
	enum_mysqlnd_query_type			last_query_type;
	/* Temporary storage between query and (use|store)_result() call */
	MYSQLND_RES						*current_result;

	/*
	  How many result sets reference this connection.
	  It won't be freed until this number reaches 0.
	  The last one, please close the door! :-)
	  The result set objects can determine by inspecting
	  'quit_sent' whether the connection is still valid.
	*/
	unsigned int	refcount;

	/* Temporal storage for mysql_query */
	unsigned int	field_count;

	/* persistent connection */
	zend_bool		persistent;

	/* options */
	MYSQLND_OPTIONS	options;

	/* stats */
	MYSQLND_STATS	* stats;

	struct st_mysqlnd_conn_methods *m;
};


struct mysqlnd_field_hash_key
{
	zend_bool		is_numeric;
	unsigned long	key;
#if MYSQLND_UNICODE
	zstr			ustr;
	unsigned int	ulen;
#endif
};


struct st_mysqlnd_result_metadata
{
	MYSQLND_FIELD					*fields;
	struct mysqlnd_field_hash_key	*zend_hash_keys;
	unsigned int					current_field;
	unsigned int					field_count;
	/* We need this to make fast allocs in rowp_read */
	unsigned int					bit_fields_count;
	size_t							bit_fields_total_len; /* trailing \0 not counted */
	zend_bool						persistent;

	struct st_mysqlnd_res_meta_methods *m;
};


struct st_mysqlnd_buffered_result
{
	zval				**data;
	zval				**data_cursor;
	MYSQLND_MEMORY_POOL_CHUNK **row_buffers;
	uint64_t			row_count;
	uint64_t			initialized_rows;
	zend_bool			persistent;

	unsigned int		references;

	MYSQLND_ERROR_INFO	error_info;
};


struct st_mysqlnd_unbuffered_result
{
	/* For unbuffered (both normal and PS) */
	zval				**last_row_data;
	MYSQLND_MEMORY_POOL_CHUNK *last_row_buffer;

	uint64_t			row_count;
	zend_bool			eof_reached;
};


struct st_mysqlnd_res
{
	struct st_mysqlnd_res_methods m;

	MYSQLND					*conn;
	enum_mysqlnd_res_type	type;
	unsigned int			field_count;

	/* For metadata functions */
	MYSQLND_RES_METADATA	*meta;

	/* To be used with store_result() - both normal and PS */
	MYSQLND_RES_BUFFERED		*stored_data;
	MYSQLND_RES_UNBUFFERED		*unbuf;

	/*
	  Column lengths of current row - both buffered and unbuffered.
	  For buffered results it duplicates the data found in **data
	*/
	unsigned long			*lengths;

	struct st_mysqlnd_packet_row * row_packet;

	MYSQLND_MEMORY_POOL		* result_set_memory_pool;
	zend_bool				persistent;
};


struct st_mysqlnd_param_bind
{
	zval		*zv;
	zend_uchar	type;
	enum_param_bind_flags	flags;
};

struct st_mysqlnd_result_bind
{
	zval		*zv;
	zend_bool	bound;
};


struct st_mysqlnd_stmt_data
{
	MYSQLND						*conn;
	unsigned long				stmt_id;
	unsigned long				flags;/* cursor is set here */
	enum_mysqlnd_stmt_state		state;
	unsigned int				warning_count;
	MYSQLND_RES					*result;
	unsigned int				field_count;
	unsigned int				param_count;
	unsigned char				send_types_to_server;
	MYSQLND_PARAM_BIND			*param_bind;
	MYSQLND_RESULT_BIND			*result_bind;
	zend_bool					result_zvals_separated_once;
	zend_bool					persistent;

	MYSQLND_UPSERT_STATUS		upsert_status;

	MYSQLND_ERROR_INFO			error_info;

	zend_bool					update_max_length;
	unsigned long				prefetch_rows;

	zend_bool					cursor_exists;
	mysqlnd_stmt_use_or_store_func default_rset_handler;

	MYSQLND_CMD_BUFFER			execute_cmd_buffer;
	unsigned int				execute_count;/* count how many times the stmt was executed */
};


struct st_mysqlnd_stmt
{
	MYSQLND_STMT_DATA * data;
	struct st_mysqlnd_stmt_methods	*m;
	zend_bool persistent;
};

#endif /* MYSQLND_STRUCTS_H */
