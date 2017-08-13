#include <math.h>
#include <string.h>
#include <assert.h>

#include <arpa/inet.h>

#include <pthread.h>
#include <vector>
#include <string>
#include <map>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "eproto.h"

#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )

#define htonll(x) ntohll(x)

#define MP_ENABLE_DEBUGPRINT 0

#if MP_ENABLE_DEBUGPRINT
#define MPDEBUGPRINT(...) fprintf( stderr, __VA_ARGS__ )
#else
#define MPDEBUGPRINT(...) 
#endif

// 4194304=4M 67108864＝64M 1048576=1M
#define MSG_PACK_EXPAND_LENGTH 4194304

typedef struct mpwbuf_t {
    size_t used; // start from zero
//    size_t capacity;
    int err;
    unsigned char buffer[MSG_PACK_EXPAND_LENGTH];
//    unsigned char data[1024*1024];
} mpwbuf_t;

#define ERRORBIT_BUFNOLEFT 1
#define ERRORBIT_STRINGLEN 2
#define ERRORBIT_TYPE_LIGHTUSERDATA 4
#define ERRORBIT_TYPE_FUNCTION 8
#define ERRORBIT_TYPE_USERDATA 16
#define ERRORBIT_TYPE_THREAD 32
#define ERRORBIT_TYPE_UNKNOWN 64
#define ERRORBIT_TYPE_NO_PROTO 128
#define ERRORBIT_TYPE_WRONG_PROTO 256

// 管理空闲Buffer
typedef std::vector<mpwbuf_t*> mpwbufVector;
pthread_mutex_t g_mutex;
mpwbufVector g_idles;

mpwbuf_t* get_idle_buffer(){
	mpwbuf_t* p_mpwbuf;
	pthread_mutex_lock(&g_mutex);
	if(g_idles.empty()){
		p_mpwbuf = new mpwbuf_t();
	}else{
		p_mpwbuf = g_idles.back();
		g_idles.pop_back();
	}
	pthread_mutex_unlock(&g_mutex);
	return p_mpwbuf;
}
void set_idle_buffer(mpwbuf_t* p_mpwbuf){
	pthread_mutex_lock(&g_mutex);
	g_idles.push_back(p_mpwbuf);
	pthread_mutex_unlock(&g_mutex);
}

#define PROTO_TYPE_LUA_TABLE 0
#define PROTO_TYPE_PROTO_TABLE 1
#define PROTO_TYPE_PROTO_ARRAY 2
#define PROTO_TYPE_ELEMENT 3

// 管理proto数据
typedef struct proto_element{
	unsigned int type : 8;
	unsigned int index : 24;
	unsigned int msgID;
	std::string name;
//	std::string msg;
}proto_element;
typedef std::vector<proto_element*> proto_element_vector;
typedef std::vector<proto_element_vector*> proto_vector;
typedef std::map<std::string, size_t> proto_map;
//typedef std::map<std::string, proto_element_vector*> proto_map;
//proto_map g_protos;
proto_map g_proto_index;
proto_vector g_protos;
pthread_mutex_t g_proto_mutex;
void release_proto_element_vector(proto_element_vector* pVec){
	for( auto pElement : (*pVec) ){
		if(NULL != pElement){
			delete pElement;
		}
	}
	delete pVec;
}
proto_element_vector* get_proto_info(const std::string& msg){
	proto_element_vector* pVec = NULL;
	proto_map::iterator itCur;
	pthread_mutex_lock(&g_proto_mutex);
	itCur = g_proto_index.find(msg);
	if(itCur != g_proto_index.end()){
		pVec = g_protos[itCur->second];
	}
	pthread_mutex_unlock(&g_proto_mutex);
	return pVec;
}
proto_element_vector* get_proto_info_by_id(size_t id){
	proto_element_vector* pVec = NULL;
	pthread_mutex_lock(&g_proto_mutex);
	if(id < g_protos.size()){
		pVec = g_protos[id];
	}
	pthread_mutex_unlock(&g_proto_mutex);
	return pVec;
}
size_t get_proto_id(const std::string& msg){
	size_t id = 0;
	proto_map::iterator itCur;
	pthread_mutex_lock(&g_proto_mutex);
	itCur = g_proto_index.find(msg);
	if(itCur != g_proto_index.end()){
		id = itCur->second;
	}
	pthread_mutex_unlock(&g_proto_mutex);
	return id;
}

bool set_proto_info(const std::string& msg, proto_element_vector* pVec, unsigned int* index){
	bool ret = false;
	proto_element_vector* pOldVec;
	proto_map::iterator itCur;
	proto_element* pElement;
	pthread_mutex_lock(&g_proto_mutex);
	itCur = g_proto_index.find(msg);
	if(itCur == g_proto_index.end()){
		(*index) = g_protos.size();
		g_proto_index.insert(std::make_pair(msg, (*index)));
		g_protos.push_back(pVec);
		ret = true;
	}else{
		(*index) = itCur->second;
		pOldVec = g_protos[(*index)];
		// 合并这两个数据，以旧数据为准，在此基础上面增加
		if(pVec->size() > pOldVec->size()){
			pOldVec->resize(pVec->size(), NULL);
		}
		for(size_t i=0; i<pVec->size(); ++i){
			pElement = (*pVec)[i];
			if(NULL == (*pOldVec)[i] && pElement != NULL){
				(*pOldVec)[i] = pElement;
				(*pVec)[i] = NULL;
			}
		}
	}
	pthread_mutex_unlock(&g_proto_mutex);
	return ret;
}
bool create_proto_element(int type, int index, proto_element_vector* pVec, proto_element** pe){
	if(index >= (int)pVec->size()){
		pVec->resize(index + 1, NULL);
	}
	(*pe) = pVec->at(index);
	if(NULL == (*pe)){
		(*pe) = new proto_element();
		(*pe)->type = type;
		(*pe)->index = index;
		(*pe)->msgID = 0;
		(*pVec)[index] = (*pe);
		return true;
	}
	return false;
}
bool set_proto_info_table(lua_State *L, int index, proto_element_vector* pVec){
	// 获取-1中的info数据，解析保存到vector中
	size_t l = lua_objlen(L,index);
	int i,t;
	size_t len;
	int p_type;
	const char* key;
	proto_element* pe;
	for(i=1;i<=(int)l;i++){
		lua_rawgeti(L,index,i); // push table value to stack
		t = lua_type(L,-1);
		if(t == LUA_TSTRING){
			if( create_proto_element(PROTO_TYPE_ELEMENT, i-1, pVec, &pe) ){
    			key = lua_tolstring(L,-1,&len);
    			std::string keystr(key, len);
    			pe->name.swap(keystr);
			}
		}else if(t == LUA_TTABLE){
			lua_rawgeti(L,-1,1); // push table value to stack
			p_type = (int)lua_tonumber(L,-1);
			lua_pop(L,1); // repair stack
			if( !(p_type == PROTO_TYPE_PROTO_ARRAY || p_type == PROTO_TYPE_PROTO_TABLE) ){
				p_type = PROTO_TYPE_LUA_TABLE;
			}
			if( create_proto_element(p_type, i-1, pVec, &pe) ){
				lua_rawgeti(L,-1,2); // push table value to stack
				key = lua_tolstring(L,-1,&len);
				std::string keystr(key, len);
				pe->name.swap(keystr);
				lua_pop(L,1); // repair stack
				if (p_type == PROTO_TYPE_PROTO_ARRAY || p_type == PROTO_TYPE_PROTO_TABLE){
					lua_rawgeti(L,-1,3); // push table value to stack
					key = lua_tolstring(L,-1,&len);
					std::string msgstr(key, len);
					lua_pop(L,1); // repair stack
					pe->msgID = get_proto_id(msgstr);
					// 如果这个名称的proto还没有生成，那么先生成一个占位
					if(pe->msgID == 0){
						proto_element_vector* pOtherVec = new proto_element_vector();
						if( !set_proto_info(msgstr, pOtherVec, &pe->msgID) ){
							release_proto_element_vector(pOtherVec);
						}
					}
					//pe->msg.swap(msgstr);
				}
			}
		}else{
			return false;
		}
		lua_pop(L,1); // repair stack
	}
	return true;
}
static int msgpack_proto_api(lua_State *L){
	size_t slen;
	const char *sval = lua_tolstring(L,1,&slen);
	std::string msg(sval, slen);
	proto_element_vector* pVec = new proto_element_vector();
	unsigned int index = 0;
	bool ret = set_proto_info_table(L, 2, pVec);
	if(ret){
		if( !set_proto_info(msg, pVec, &index) ){
			release_proto_element_vector(pVec);
		}
	}else{
		release_proto_element_vector(pVec);
		fprintf(stderr, "set_proto_info_table failed\n");
	}
	lua_pushnumber(L, index);
	return 1;
}
static int msgpack_protoID_api(lua_State *L){
	size_t slen;
	const char *sval = lua_tolstring(L,1,&slen);
	std::string msg(sval, slen);
	size_t id = get_proto_id(msg);
	lua_pushnumber(L, id);
	return 1;
}


typedef struct {
    unsigned char *data; // read buffer dont have to allocate buffer.
    size_t ofs;
    size_t len;
    int err;
} mprbuf_t;

//mpwbuf_t g_mpwbuf; // single threaded!

void mpwbuf_init(mpwbuf_t *b){
    b->used=0;
//    b->capacity = sizeof(b->data);
    b->err=0;
}
void mprbuf_init(mprbuf_t *b, const unsigned char *p, size_t len ){
    b->data = (unsigned char*)p;
    b->len = len;
    b->ofs=0;
    b->err=0;
}

//size_t mpwbuf_left(mpwbuf_t *b){
//    return b->capacity - b->used;
//}
size_t mprbuf_left(mprbuf_t *b){
    return b->len - b->ofs;
}

void mp_rcopy( unsigned char *dest, unsigned char*from, size_t l ){
    size_t i;
    for(i=0;i<l;i++){
        dest[l-i-1]=from[i];
    }
}

size_t mpwbuf_append(mpwbuf_t *b, const unsigned char *toadd, size_t l){
    if(b->err){return 0;}
//    if( mpwbuf_left(b)<l){
//        b->err |= ERRORBIT_BUFNOLEFT;
//        return 0;
//    }
//    memcpy( b->data + b->used, toadd,l);
//    b->used += l;

	size_t len = b->used + l;
	if(len > MSG_PACK_EXPAND_LENGTH){
        b->err |= ERRORBIT_BUFNOLEFT;
        return 0;
	}
    memcpy( b->buffer + b->used, toadd, l);
	b->used = len;

    return l;
}
// return packed size. 0 when error.
static size_t mpwbuf_pack_nil( mpwbuf_t *b ){
    unsigned char append[1]={ 0xc0 };    
    return mpwbuf_append(b,append,1);
}
static size_t mpwbuf_pack_boolean( mpwbuf_t *b, int i) {
    unsigned char append[1];
    if(i){
        append[0] = 0xc3;
    } else {
        append[0] = 0xc2;
    }
    return mpwbuf_append(b,append,1);
}
static size_t mpwbuf_pack_number( mpwbuf_t *b, lua_Number n ) {
    unsigned char buf[1+8];
    size_t len=0;

    if( isinf(n) ){
        buf[0] = 0xcb; // double
        if(n>0){
            buf[1] = 0x7f;
            buf[2] = 0xf0;
        } else {
            buf[1] = 0xff;
            buf[2] = 0xf0;
        }
        buf[3] = buf[4] = buf[5] = buf[6] = buf[7] = buf[8] = 0;
        len += 1+8;
    } else if( isnan(n) ) {
        buf[0] = 0xcb;
        buf[1] = 0xff;
        buf[2] = 0xf8;
        buf[3] = buf[4] = buf[5] = buf[6] = buf[7] = buf[8] = 0;
        len += 1+8;        
    } else if(floor(n)==n){
        long long lv = (long long)n;
        if(lv>=0){
            if(lv<128){
                buf[0]=(char)lv;
                len=1;
            } else if(lv<256){
                buf[0] = 0xcc;
                buf[1] = (char)lv;
                len=2;
            } else if(lv<65536){
                buf[0] = 0xcd;
                short v = htons((short)lv);
                memcpy(buf+1,&v,2);
                len=1+2;
            } else if(lv<4294967296LL){
                buf[0] = 0xce;
                long v = htonl((long)lv);
                memcpy(buf+1,&v,4);
                len=1+4;
            } else {
                buf[0] = 0xcf;
                long long v = htonll((long long)lv);
                memcpy(buf+1,&v,8);
                len=1+8;
            }
        } else {
            if(lv >= -32){
                buf[0] = 0xe0 | (char)lv;
                len=1;
            } else if( lv >= -128 ){
                buf[0] = 0xd0;
                buf[1] = lv;
                len=2;
            } else if( lv >= -32768 ){
                short v = htons(lv&0xffff);
                buf[0] = 0xd1;
                memcpy(buf+1, &v,2);
                len=1+2;
            } else if( lv >= -2147483648LL ){
                int v = htonl(lv&0xffffffff);
                buf[0] = 0xd2;
                memcpy(buf+1,&v,4);
                len=1+4;
            } else{
                long long v = htonll(lv);
                buf[0] = 0xd3;
                memcpy(buf+1,&v,8);
                len=1+8;
            }
        }
    } else { // floating point!
        assert(sizeof(double)==sizeof(n));
        buf[0] = 0xcb;
        mp_rcopy(buf+1,(unsigned char*)&n,sizeof(double)); // endianness
        len=1+8;
    }
    return mpwbuf_append(b,buf,len);
}
static size_t mpwbuf_pack_string( mpwbuf_t *b, const unsigned char *sval, size_t slen ) {
    unsigned char topbyte=0;
    size_t wl=0;
    if(slen<32){
        topbyte = 0xa0 | (char)slen;
        wl = mpwbuf_append(b, &topbyte, 1 );
        wl += mpwbuf_append(b, sval, slen);
    } else if(slen<65536){
        topbyte = 0xda;
        wl = mpwbuf_append(b,&topbyte,1);
        unsigned short l = htons(slen);
        wl += mpwbuf_append(b,(unsigned char*)(&l),2);
        wl += mpwbuf_append(b,sval,slen);
    } else if(slen<4294967296LL-1){ // TODO: -1 for avoiding (condition is always true warning)
        topbyte = 0xdb;
        wl = mpwbuf_append(b,&topbyte,1);
        unsigned int l = htonl(slen);
        wl += mpwbuf_append(b,(unsigned char*)(&l),4);
        wl += mpwbuf_append(b,sval,slen);
    } else {
        b->err |= ERRORBIT_STRINGLEN;
    }
    return wl;    
}

static size_t mpwbuf_pack_anytype( mpwbuf_t *b, lua_State *L, int index ) ;


// from mplua:  void packTable(Packer& pk, int index) const {
// check if this is an array
// NOTE: This code strongly depends on the internal implementation
// of Lua5.1. The table in Lua5.1 consists of two parts: the array part
// and the hash part. The array part is placed before the hash part.
// Therefore, it is possible to obtain the first key of the hash part
// by using the return value of lua_objlen as the argument of lua_next.
// If lua_next return 0, it means the table does not have the hash part,
// that is, the table is an array.
//
// Due to the specification of Lua, the table with non-continous integral
// keys is detected as a table, not an array.

// index: index in stack
static size_t mpwbuf_pack_table( mpwbuf_t *b, lua_State *L, int index ) {
    size_t nstack = lua_gettop(L);
    size_t l = lua_objlen(L,index);
    
    size_t wl=0;
    //    fprintf(stderr, "mpwbuf_pack_table: lua_objlen: array part len: %d index:%d\n", (int)l,index);
    
    // try array first, and then map.
    if(l>0){
        unsigned char topbyte;
        // array!(ignore map part.) 0x90|n , 0xdc+2byte, 0xdd+4byte
        if(l<16){
            topbyte = 0x90 | (unsigned char)l;
            wl = mpwbuf_append(b,&topbyte,1);
        } else if( l<65536){
            topbyte = 0xdc;
            wl = mpwbuf_append(b,&topbyte,1);
            unsigned short elemnum = htons(l);
            wl += mpwbuf_append(b,(unsigned char*)&elemnum,2);
        } else if( l<4294967296LL-1){ // TODO: avoid C warn
            topbyte = 0xdd;
            wl = mpwbuf_append(b,&topbyte,1);
            unsigned int elemnum = htonl(l);
            wl += mpwbuf_append(b,(unsigned char*)&elemnum,4);
        }
        
        int i;
        for(i=1;i<=(int)l;i++){
            lua_rawgeti(L,index,i); // push table value to stack
            wl += mpwbuf_pack_anytype(b,L,nstack+1);
            lua_pop(L,1); // repair stack
        }
    } else {
        // map!
        l=0;
        lua_pushnil(L);
        while(lua_next(L,index)){
            l++;
            lua_pop(L,1);
        }
        // map fixmap, 16,32 : 0x80|num, 0xde+2byte, 0xdf+4byte
        unsigned char topbyte=0;
        if(l<16){
            topbyte = 0x80 | (char)l;
            wl = mpwbuf_append(b,&topbyte,1);
        }else if(l<65536){
            topbyte = 0xde;
            wl = mpwbuf_append(b,&topbyte,1);
            unsigned short elemnum = htons(l);
            wl += mpwbuf_append(b,(unsigned char*)&elemnum,2);
        }else if(l<4294967296LL-1){
            topbyte = 0xdf;
            wl = mpwbuf_append(b,&topbyte,1);
            unsigned int elemnum = htonl(l);
            wl += mpwbuf_append(b,(unsigned char*)&elemnum,4);
        }
        lua_pushnil(L); // nil for first iteration on lua_next
        while( lua_next(L,index)){
            wl += mpwbuf_pack_anytype(b,L,nstack+1); // -2:key
            wl += mpwbuf_pack_anytype(b,L,nstack+2); // -1:value
            lua_pop(L,1); // remove value and keep key for next iteration
        }
    }
    return wl;
}

static size_t mpwbuf_pack_anytype( mpwbuf_t *b, lua_State *L, int index ) {
    //    int top = lua_gettop(L); 
    //    fprintf(stderr, "mpwbuf_pack_anytype: top:%d index:%d\n", top, index);
    int t = lua_type(L,index);
    switch(t){
    case LUA_TNUMBER:
        {
            lua_Number nv = lua_tonumber(L,index);
//            return mpwbuf_pack_number(&g_mpwbuf,nv);
            return mpwbuf_pack_number(b,nv);
        }
        break;
    case LUA_TSTRING:
        {
            size_t slen;
            const char *sval = luaL_checklstring(L,index,&slen);
//            return mpwbuf_pack_string(&g_mpwbuf,(const unsigned char*)sval,slen);
            return mpwbuf_pack_string(b,(const unsigned char*)sval,slen);
        }
        break;
    case LUA_TNIL:
//        return mpwbuf_pack_nil(&g_mpwbuf);
        return mpwbuf_pack_nil(b);
    case LUA_TBOOLEAN:
        {
            int iv = lua_toboolean(L,index);
//            return mpwbuf_pack_boolean(&g_mpwbuf,iv);
            return mpwbuf_pack_boolean(b,iv);
        }
    case LUA_TTABLE:
//        return mpwbuf_pack_table(&g_mpwbuf, L, index );
        return mpwbuf_pack_table(b, L, index );
    case LUA_TLIGHTUSERDATA:
        b->err |= ERRORBIT_TYPE_LIGHTUSERDATA;
        break;
    case LUA_TFUNCTION:
        b->err |= ERRORBIT_TYPE_FUNCTION;
        break;
    case LUA_TUSERDATA:
        b->err |= ERRORBIT_TYPE_USERDATA;
        break;
    case LUA_TTHREAD:
        b->err |= ERRORBIT_TYPE_THREAD;
        break;
    default:
        b->err |= ERRORBIT_TYPE_UNKNOWN;
        break;
    }

    return 0;    
}

static size_t msgpack_encode_proto(mpwbuf_t *b, lua_State *L, int index, proto_element_vector* pVec);
size_t msgpack_encode_proto_array(mpwbuf_t *b, lua_State *L, int index, proto_element_vector* pVec){
	// 判断当前数据是否是nil
	int t = lua_type(L,index);
	if(t == LUA_TNIL){
		return mpwbuf_pack_nil(b);
	}
	//获取数组的长度写入
	size_t l = lua_objlen(L,index);
	size_t wl=0;
	unsigned char topbyte;
	// array!(ignore map part.) 0x90|n , 0xdc+2byte, 0xdd+4byte
	if(l<16){
		topbyte = 0x90 | (unsigned char)l;
		wl = mpwbuf_append(b,&topbyte,1);
	} else if( l<65536){
		topbyte = 0xdc;
		wl = mpwbuf_append(b,&topbyte,1);
		unsigned short elemnum = htons(l);
		wl += mpwbuf_append(b,(unsigned char*)&elemnum,2);
	} else if( l<4294967296LL-1){ // TODO: avoid C warn
		topbyte = 0xdd;
		wl = mpwbuf_append(b,&topbyte,1);
		unsigned int elemnum = htonl(l);
		wl += mpwbuf_append(b,(unsigned char*)&elemnum,4);
	}
	// 写入数组中的proto数据
	int value_index = index + 1;
	for(size_t i=1; i<=l; ++i){
		lua_rawgeti(L,index,i); // push table value to stack
		wl += msgpack_encode_proto(b, L, value_index, pVec);
		lua_pop(L,1); // repair stack
	}
	return wl;
}

static size_t msgpack_encode_proto(mpwbuf_t *b, lua_State *L, int index, proto_element_vector* pVec){
	int t = lua_type(L,index);
	if(t == LUA_TNIL){
		return mpwbuf_pack_nil(b);
	}
	if( t != LUA_TTABLE ){
		b->err |= ERRORBIT_TYPE_WRONG_PROTO;
		return 0;
	}
	if(NULL == pVec){
		b->err |= ERRORBIT_TYPE_NO_PROTO;
		return 0;
	}
	int value_index = index + 1;
	size_t wl=0;
	// 写入数组长度
	size_t l = pVec->size();
	unsigned char topbyte;
	// array!(ignore map part.) 0x90|n , 0xdc+2byte, 0xdd+4byte
	if(l<16){
		topbyte = 0x90 | (unsigned char)l;
		wl = mpwbuf_append(b,&topbyte,1);
	} else if( l<65536){
		topbyte = 0xdc;
		wl = mpwbuf_append(b,&topbyte,1);
		unsigned short elemnum = htons(l);
		wl += mpwbuf_append(b,(unsigned char*)&elemnum,2);
	} else if( l<4294967296LL-1){ // TODO: avoid C warn
		topbyte = 0xdd;
		wl = mpwbuf_append(b,&topbyte,1);
		unsigned int elemnum = htonl(l);
		wl += mpwbuf_append(b,(unsigned char*)&elemnum,4);
	}
	// 根据pVec的信息，获取key对应的数据，写入
	proto_element* pe;
	for(size_t i=0; i<l; ++i){
		pe = pVec->at(i);
		if(NULL == pe){
			wl += mpwbuf_pack_nil(b);
		}else{
			// 查找name对应的字段的值
			lua_pushlstring(L, pe->name.c_str(), pe->name.length()); // [table, key]
			lua_gettable(L, -2); // [table, value]
			switch(pe->type){
				case PROTO_TYPE_ELEMENT:{
					wl += mpwbuf_pack_anytype(b, L, value_index); // -1:value
					break;
				}
				case PROTO_TYPE_PROTO_TABLE:{
					proto_element_vector* pOtherVec = get_proto_info_by_id(pe->msgID);
					wl += msgpack_encode_proto(b, L, value_index, pOtherVec);
					break;
				}
				case PROTO_TYPE_PROTO_ARRAY:{
					proto_element_vector* pOtherVec = get_proto_info_by_id(pe->msgID);
					if(NULL == pOtherVec){
						b->err |= ERRORBIT_TYPE_NO_PROTO;
						return 0;
					}else{
						wl += msgpack_encode_proto_array(b, L, value_index, pOtherVec);
					}
					break;
				}
				case PROTO_TYPE_LUA_TABLE:{
					wl += mpwbuf_pack_anytype(b, L, value_index); // -1:value
					break;
				}
				default:{
					wl += mpwbuf_pack_nil(b);
					break;
				}
			}
			lua_pop(L,1); // repair stack
		}
	}
	return wl;
}

static int msgpack_encode_api( lua_State *L ){
	mpwbuf_t *p_mpwbuf = get_idle_buffer();
    mpwbuf_init( p_mpwbuf );
    size_t id;
	int t = lua_type(L,1);
	if(t == LUA_TNUMBER){
		id = lua_tonumber(L, 1);
	}else{
		size_t slen;
		const char *sval = lua_tolstring(L,1,&slen);
		std::string msg(sval, slen);
		id = get_proto_id(msg);
	}
	proto_element_vector* pVec = get_proto_info_by_id(id);
	size_t wlen = msgpack_encode_proto(p_mpwbuf, L, 2, pVec);
    if(wlen>0 && p_mpwbuf->err == 0 ){
        lua_pushlstring(L,(const char*)(p_mpwbuf->buffer),p_mpwbuf->used);
		set_idle_buffer(p_mpwbuf);
        return 1;
    } else {
        const char *errmsg = "unknown error";
        if( p_mpwbuf->err & ERRORBIT_BUFNOLEFT ){
            errmsg = "no buffer left";
        } else if ( p_mpwbuf->err &  ERRORBIT_STRINGLEN ){
            errmsg = "string too long";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_LIGHTUSERDATA ){
            errmsg = "invalid type: lightuserdata";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_FUNCTION ){
            errmsg = "invalid type: function";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_USERDATA ){
            errmsg = "invalid type: userdata";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_THREAD ){
            errmsg = "invalid type: thread";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_UNKNOWN ){
            errmsg = "invalid type: unknown";
        } else if( p_mpwbuf->err & ERRORBIT_TYPE_NO_PROTO ){
        	errmsg = "proto not found";
        } else if( p_mpwbuf->err & ERRORBIT_TYPE_WRONG_PROTO ){
        	errmsg = "proto should be a lua table to encode";
        }

        lua_pushfstring( L, errmsg );
        lua_error(L);
		set_idle_buffer(p_mpwbuf);
        return 2;
    }
}

// return num of return value
static int msgpack_pack_api( lua_State *L ) {
//	mpwbuf_t mpwbuf;
	mpwbuf_t *p_mpwbuf = get_idle_buffer();
    mpwbuf_init( p_mpwbuf );
    size_t wlen = mpwbuf_pack_anytype(p_mpwbuf,L,1);
    if(wlen>0 && p_mpwbuf->err == 0 ){
//        lua_pushlstring(L,(const char*)p_mpwbuf.data,p_mpwbuf.used);
        lua_pushlstring(L,(const char*)(p_mpwbuf->buffer),p_mpwbuf->used);
		set_idle_buffer(p_mpwbuf);
        return 1;
    } else {
        const char *errmsg = "unknown error";
        if( p_mpwbuf->err & ERRORBIT_BUFNOLEFT ){
            errmsg = "no buffer left";
        } else if ( p_mpwbuf->err &  ERRORBIT_STRINGLEN ){
            errmsg = "string too long";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_LIGHTUSERDATA ){
            errmsg = "invalid type: lightuserdata";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_FUNCTION ){
            errmsg = "invalid type: function";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_USERDATA ){
            errmsg = "invalid type: userdata";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_THREAD ){
            errmsg = "invalid type: thread";
        } else if ( p_mpwbuf->err & ERRORBIT_TYPE_UNKNOWN ){
            errmsg = "invalid type: unknown";
        }

        lua_pushfstring( L, errmsg );
        lua_error(L);
		set_idle_buffer(p_mpwbuf);
        return 2;
    }

//    mpwbuf_init( &g_mpwbuf);
//    size_t wlen = mpwbuf_pack_anytype(&g_mpwbuf,L,1);
//    if(wlen>0 && g_mpwbuf.err == 0 ){
//        lua_pushlstring(L,(const char*)g_mpwbuf.data,g_mpwbuf.used);
//        return 1;
//    } else {
//        const char *errmsg = "unknown error";
//        if( g_mpwbuf.err & ERRORBIT_BUFNOLEFT ){
//            errmsg = "no buffer left";
//        } else if ( g_mpwbuf.err &  ERRORBIT_STRINGLEN ){
//            errmsg = "string too long";
//        } else if ( g_mpwbuf.err & ERRORBIT_TYPE_LIGHTUSERDATA ){
//            errmsg = "invalid type: lightuserdata";
//        } else if ( g_mpwbuf.err & ERRORBIT_TYPE_FUNCTION ){
//            errmsg = "invalid type: function";
//        } else if ( g_mpwbuf.err & ERRORBIT_TYPE_USERDATA ){
//            errmsg = "invalid type: userdata";
//        } else if ( g_mpwbuf.err & ERRORBIT_TYPE_THREAD ){
//            errmsg = "invalid type: thread";
//        } else if ( g_mpwbuf.err & ERRORBIT_TYPE_UNKNOWN ){
//            errmsg = "invalid type: unknown";
//        }
//
//        lua_pushfstring( L, errmsg );
//        lua_error(L);
//        return 2;
//    }
}

// push a table
static void mprbuf_unpack_anytype( mprbuf_t *b, lua_State *L );
static void mprbuf_unpack_array( mprbuf_t *b, lua_State *L, int arylen ) {
    //    lua_newtable(L);
    lua_createtable(L,arylen,0);
    int i;
    for(i=0;i<arylen;i++){
        mprbuf_unpack_anytype(b,L); // array element
        if(b->err)break;
        lua_rawseti(L, -2, i+1);
    }
}
static void mprbuf_unpack_map( mprbuf_t *b, lua_State *L, int maplen ) {
    // return a table
    //    lua_newtable(L); // push {}
    lua_createtable(L,0,maplen);
    int i;
    for(i=0;i<maplen;i++){
        mprbuf_unpack_anytype(b,L); // key
        mprbuf_unpack_anytype(b,L); // value
        lua_rawset(L,-3);
    }
}

static void mprbuf_unpack_anytype( mprbuf_t *b, lua_State *L ) {
    if( mprbuf_left(b) < 1){
        b->err |= 1;
        return;
    }
    unsigned char t = b->data[ b->ofs ];
    //        fprintf( stderr, "mprbuf_unpack_anytype: topbyte:%x ofs:%d len:%d\n",(int)t, (int)b->ofs, (int)b->len );

    b->ofs += 1; // for toptypebyte

    if(t<0x80){ // fixed num
        lua_pushnumber(L,(lua_Number)t);
        return;
    }

    unsigned char *s = b->data + b->ofs;

    if(t>=0x80 && t <=0x8f){ // fixed map
        size_t maplen = t & 0xf;
        mprbuf_unpack_map(b,L,maplen);
        return;
    }
    if(t>=0x90 && t <=0x9f){ // fixed array
        size_t arylen = t & 0xf;
        mprbuf_unpack_array(b,L,arylen);
        return;
    }

    if(t>=0xa0 && t<=0xbf){ // fixed string
        size_t slen = t & 0x1f;
        if( mprbuf_left(b) < slen ){
            b->err |= 1;
            return;
        }
        lua_pushlstring(L,(const char*)s,slen);
        b->ofs += slen;
        return;
    }
    if(t>0xdf){ // fixnum_neg (-32 ~ -1)
        unsigned char ut = t;
        lua_Number n = ( 256 - ut ) * -1;
        lua_pushnumber(L,n);
        return;
    }

    switch(t){
    case 0xc0: // nil
        lua_pushnil(L);
        return;
    case 0xc2: // false
        lua_pushboolean(L,0);
        return;
    case 0xc3: // true
        lua_pushboolean(L,1);
        return;

    case 0xca: // float
        if(mprbuf_left(b)>=4){
            float f;
            mp_rcopy( (unsigned char*)(&f), s,4); // endianness
            lua_pushnumber(L,f);
            b->ofs += 4;
            return;
        }
        break;
    case 0xcb: // double
        if(mprbuf_left(b)>=8){
            double v;
            mp_rcopy( (unsigned char*)(&v), s,8); // endianness
            lua_pushnumber(L,v);
            b->ofs += 8;
            return;
        }
        break;

    case 0xcc: // 8bit large posi int
        if(mprbuf_left(b)>=1){
            lua_pushnumber(L,(unsigned char) s[0] );
            b->ofs += 1;
            return;
        }
        break;
    case 0xcd: // 16bit posi int
        if(mprbuf_left(b)>=2){
            unsigned short v = ntohs( *(short*)(s) );
            lua_pushnumber(L,v);
            b->ofs += 2;
            return;
        }
        break;
    case 0xce: // 32bit posi int
        if(mprbuf_left(b)>=4){
            unsigned long v = ntohl( *(long*)(s) );
            lua_pushnumber(L,v);
            b->ofs += 4;
            return;
        }
        break;
    case 0xcf: // 64bit posi int
        if(mprbuf_left(b)>=8){
            unsigned long long v = ntohll( *(long long*)(s));
            lua_pushnumber(L,v);
            b->ofs += 8;
            return;
        }
        break;
    case 0xd0: // 8bit neg int
        if(mprbuf_left(b)>=1){
            lua_pushnumber(L, (signed char) s[0] );
            b->ofs += 1;
            return;
        }
        break;
    case 0xd1: // 16bit neg int
        if(mprbuf_left(b)>=2){
            short v = *(short*)(s);
            v = ntohs(v);
            lua_pushnumber(L,v);
            b->ofs += 2;
            return;
        }
        break;
    case 0xd2: // 32bit neg int
        if(mprbuf_left(b)>=4){
            int v = *(long*)(s);
            v = ntohl(v);
            lua_pushnumber(L,v);
            b->ofs += 4;
            return;
        }
        break;
    case 0xd3: // 64bit neg int
        if(mprbuf_left(b)>=8){
            long long v = *(long long*)(s);
            v = ntohll(v);
            lua_pushnumber(L,v);
            b->ofs += 8;
            return;
        }
        break;
    case 0xda: // long string len<65536
        if(mprbuf_left(b)>=2){
            size_t slen = ntohs(*((unsigned short*)(s)));
            b->ofs += 2;
            if(mprbuf_left(b)>=slen){
                lua_pushlstring(L,(const char*)b->data+b->ofs,slen);
                b->ofs += slen;
                return;
            }
        }
        break;
    case 0xdb: // longer string
        if(mprbuf_left(b)>=4){
            size_t slen = ntohl(*((unsigned int*)(s)));
            b->ofs += 4;
            if(mprbuf_left(b)>=slen){
                lua_pushlstring(L,(const char*)b->data+b->ofs,slen);
                b->ofs += slen;
                return;
            }
        }

        break;

    case 0xdc: // ary16
        if(mprbuf_left(b)>=2){
            unsigned short elemnum = ntohs( *((unsigned short*)(b->data+b->ofs) ) );
            b->ofs += 2;
            mprbuf_unpack_array(b,L,elemnum);
            return;
        }
        break;
    case 0xdd: // ary32
        if(mprbuf_left(b)>=4){
            unsigned int elemnum = ntohl( *((unsigned int*)(b->data+b->ofs)));
            b->ofs += 4;
            mprbuf_unpack_array(b,L,elemnum);
            return;
        }
        break;
    case 0xde: // map16
        if(mprbuf_left(b)>=2){
            unsigned short elemnum = ntohs( *((unsigned short*)(b->data+b->ofs)));
            b->ofs += 2;
            mprbuf_unpack_map(b,L,elemnum);
            return;
        }
        break;
    case 0xdf: // map32
        if(mprbuf_left(b)>=4){
            unsigned int elemnum = ntohl( *((unsigned int*)(b->data+b->ofs)));
            b->ofs += 4;
            mprbuf_unpack_map(b,L,elemnum);
            return;
        }
        break;
    default:
        break;
    }
    b->err |= 1;
}


static int msgpack_unpack_api( lua_State *L ) {
    size_t len;
    const char * s = luaL_checklstring(L,1,&len);
    if(!s){
        lua_pushstring(L,"arg must be a string");
        lua_error(L);
        return 2;
    }
    if(len==0){
        lua_pushnil(L);
        return 1;
    }

    mprbuf_t rb;
    mprbuf_init(&rb,(const unsigned char*)s,len);

//    lua_pushnumber(L,-123456); // push readlen and replace it later
    mprbuf_unpack_anytype(&rb,L);

    //    fprintf(stderr, "mprbuf_unpack_anytype: ofs:%d len:%d err:%d\n", (int)rb.ofs, (int)rb.len, rb.err );

    if( rb.ofs >0 && rb.err==0){
        lua_pushnumber(L,rb.ofs);
//        lua_replace(L,-3); // replace dummy len
        //        fprintf(stderr, "msgpack_unpack_api: unpacked len: %d\n", (int)rb.ofs );
        return 2;
    } else{
        //        lua_pushfstring(L,"msgpack_unpack_api: unsupported type or buffer short. error code: %d\n", rb.err );
        //        lua_error(L);
        lua_pushnil(L);
//        lua_replace(L,-3);
        lua_pushnil(L);
        lua_replace(L,-2);
        return 2;
    }
}

static void msgpack_decode_proto(mprbuf_t *b, lua_State *L, proto_element_vector* pVec);
void msgpack_decode_proto_array(mprbuf_t *b, lua_State *L, proto_element_vector* pVec){
    if( mprbuf_left(b) < 1){
        b->err |= 1;
        return;
    }
    unsigned char t = b->data[ b->ofs ];

    b->ofs += 1; // for toptypebyte
    // 检测当前的数据是不是数组
    size_t arylen = 0;
	if(t>=0x90 && t <=0x9f){ // fixed array
		arylen = t & 0xf;
	}else if(t==0xdc){
		if(mprbuf_left(b)>=2){
			arylen = ntohs( *((unsigned short*)(b->data+b->ofs) ) );
			b->ofs += 2;
		}else{
	        b->err |= 1;
            return;
		}
	}else if(t==0xdd){
        if(mprbuf_left(b)>=4){
            arylen = ntohl( *((unsigned int*)(b->data+b->ofs)));
            b->ofs += 4;
		}else{
	        b->err |= 1;
            return;
		}
	}else if(t==0xc0){ // nil
		// 协议字段本身是nil
		lua_pushnil(L);
		return;
	}
	lua_createtable(L, arylen, 0);
    for(size_t i=0;i<arylen;++i){
    	msgpack_decode_proto(b,L,pVec);	// array element
        if(b->err) return;
        lua_rawseti(L, -2, i+1);
    }
}
void msgpack_decode_proto_element(mprbuf_t *b, lua_State *L, size_t arrlen, proto_element_vector* pVec){
    size_t maplen = pVec->size();
    proto_element* pe;
	lua_createtable(L, 0, maplen);
	for(size_t i=0;i<arrlen; ++i){
		if(i < maplen){
			pe = pVec->at(i);
		}else{
			pe = NULL;
		}
		if(NULL == pe){
			lua_pushnumber(L,i+1);
			mprbuf_unpack_anytype(b,L); // array element
		}else{
			switch(pe->type){
				case PROTO_TYPE_ELEMENT:{
					lua_pushlstring(L,pe->name.c_str(), pe->name.length());
					mprbuf_unpack_anytype(b,L); // array element
					break;
				}
				case PROTO_TYPE_PROTO_TABLE:{
					lua_pushlstring(L,pe->name.c_str(), pe->name.length());
					proto_element_vector* pOtherVec = get_proto_info_by_id(pe->msgID);
					msgpack_decode_proto(b,L,pOtherVec);
					if(b->err) return;
					break;
				}
				case PROTO_TYPE_PROTO_ARRAY:{
					lua_pushlstring(L,pe->name.c_str(), pe->name.length());
					proto_element_vector* pOtherVec = get_proto_info_by_id(pe->msgID);
					if(NULL == pOtherVec){
						b->err |= ERRORBIT_TYPE_NO_PROTO;
						return;
					}else{
						msgpack_decode_proto_array(b,L,pOtherVec);
					}
					break;
				}
				case PROTO_TYPE_LUA_TABLE:{
					lua_pushlstring(L,pe->name.c_str(), pe->name.length());
					mprbuf_unpack_anytype(b,L); // array element
					break;
				}
				default:{
					lua_pushnumber(L,i+1);
					mprbuf_unpack_anytype(b,L); // array element
					break;
				}
			}
		}
		lua_rawset(L,-3);
	}
}
static void msgpack_decode_proto(mprbuf_t *b, lua_State *L, proto_element_vector* pVec){
	if(NULL == pVec){
		b->err |= ERRORBIT_TYPE_NO_PROTO;
		return;
	}
    if( mprbuf_left(b) < 1){
        b->err |= 1;
        return;
    }
    unsigned char t = b->data[ b->ofs ];
    //        fprintf( stderr, "mprbuf_unpack_anytype: topbyte:%x ofs:%d len:%d\n",(int)t, (int)b->ofs, (int)b->len );

    b->ofs += 1; // for toptypebyte
    // 检测当前的数据是不是数组
	if(t>=0x90 && t <=0x9f){ // fixed array
		size_t arylen = t & 0xf;
		msgpack_decode_proto_element(b,L,arylen,pVec);
		return;
	}
	if(t==0xdc){
		if(mprbuf_left(b)>=2){
			unsigned short elemnum = ntohs( *((unsigned short*)(b->data+b->ofs) ) );
			b->ofs += 2;
			msgpack_decode_proto_element(b,L,elemnum,pVec);
			return;
		}
	}
	if(t==0xdd){
        if(mprbuf_left(b)>=4){
            unsigned int elemnum = ntohl( *((unsigned int*)(b->data+b->ofs)));
            b->ofs += 4;
            msgpack_decode_proto_element(b,L,elemnum,pVec);
            return;
        }
	}
	// 有可能proto协议本身是nil
	if(t==0xc0){ // nil
		lua_pushnil(L);
		return;
	}
	b->err |= 1;
}

static int msgpack_decode_api( lua_State *L ) {
    size_t id;
	int t = lua_type(L,1);
	if(t == LUA_TNUMBER){
		id = lua_tonumber(L, 1);
	}else{
		size_t slen;
		const char *sval = lua_tolstring(L,1,&slen);
		std::string msg(sval, slen);
		id = get_proto_id(msg);
	}
    size_t len;
    const char * s = luaL_checklstring(L,2,&len);
    if(!s){
        lua_pushstring(L,"arg must be a string");
        lua_error(L);
        return 2;
    }
    if(len==0){
        lua_pushnil(L);
        return 1;
    }

    mprbuf_t rb;
    mprbuf_init(&rb,(const unsigned char*)s,len);

//    lua_pushnumber(L,-123456); // push readlen and replace it later

	proto_element_vector* pVec = get_proto_info_by_id(id);
    msgpack_decode_proto(&rb, L, pVec);

    if( rb.ofs >0 && rb.err==0){
        lua_pushnumber(L,rb.ofs);
//        lua_replace(L,-3); // replace dummy len
        //        fprintf(stderr, "msgpack_unpack_api: unpacked len: %d\n", (int)rb.ofs );
        return 2;
    } else{
        //        lua_pushfstring(L,"msgpack_unpack_api: unsupported type or buffer short. error code: %d\n", rb.err );
        //        lua_error(L);
        lua_pushnil(L);
//        lua_replace(L,-3);
        lua_pushnil(L);
        lua_replace(L,-2);
        return 2;
    }
}

typedef enum {
    // containers without size bytes
    MPCT_FIXARRAY,
    MPCT_FIXMAP,
    // containers with size bytes
    MPCT_ARRAY16,
    MPCT_MAP16,
    MPCT_ARRAY32,
    MPCT_MAP32,
    // direct values with size bytes
    MPCT_RAW16,
    MPCT_RAW32,    
    // direct values without size bytes
    MPCT_RAW,
    MPCT_FLOAT,
    MPCT_DOUBLE,
    MPCT_UINT8,
    MPCT_UINT16,
    MPCT_UINT32,
    MPCT_UINT64,
    MPCT_INT8,
    MPCT_INT16,
    MPCT_INT32,
    MPCT_INT64,


    
} MP_CONTAINER_TYPE;

inline int MP_CONTAINER_TYPE_is_container( MP_CONTAINER_TYPE t ) {
    if( t >= MPCT_FIXARRAY && t <= MPCT_MAP32 ) {
        return 1;
    } else {
        return 0;
    }
}

inline const char *MP_CONTAINER_TYPE_to_s( MP_CONTAINER_TYPE t ) {
    switch(t){
    case MPCT_FIXARRAY: return "fixary";
    case MPCT_FIXMAP: return "fixmap";
    case MPCT_RAW: return "fixraw";
    case MPCT_FLOAT: return "float";
    case MPCT_DOUBLE: return "double";
    case MPCT_UINT8: return "u8";
    case MPCT_UINT16: return "u16";
    case MPCT_UINT32: return "u32";
    case MPCT_UINT64: return "u64";
    case MPCT_INT8: return "i8";
    case MPCT_INT16: return "i16";
    case MPCT_INT32: return "i32";
    case MPCT_INT64: return "i64";
    case MPCT_ARRAY16: return "ary16";
    case MPCT_MAP16: return "map16";
    case MPCT_RAW16: return "raw16";
    case MPCT_ARRAY32: return "ary32";
    case MPCT_MAP32: return "map32";
    case MPCT_RAW32: return "raw32";        
    default:
        assert( !"not impl");
    }
}
inline int MP_CONTAINER_TYPE_sizesize( MP_CONTAINER_TYPE t) {
    if( t == MPCT_ARRAY16 || t == MPCT_MAP16 || t == MPCT_RAW16 ){
        return 2;
    } else if( t == MPCT_ARRAY32 || t == MPCT_MAP32 || t == MPCT_RAW32 ){
        return 4;
    } else {
        return 0;
    }
}
inline int MP_CONTAINER_TYPE_is_map( MP_CONTAINER_TYPE t) {
    if( t == MPCT_FIXMAP || t == MPCT_MAP16 || t == MPCT_MAP32 ) return 1; else return 0;
}

typedef struct {
    MP_CONTAINER_TYPE t;
    size_t expect;
    size_t sofar;

    // for containers with size bytes
    char sizebytes[4];
    size_t sizesize; // array16,map16,raw16:2 array32,map32,raw32:4    
    size_t sizesofar;
} mpstackent_t;

typedef struct {
    char *buf;
    size_t size;
    size_t used;
    
    mpstackent_t stack[256];
    size_t nstacked;
    size_t resultnum;
} unpacker_t;

void unpacker_init( lua_State *L, unpacker_t *u, size_t maxsize ) {
    void* ud;
    lua_Alloc alloc = lua_getallocf( L, &ud );
    u->nstacked = 0;
    u->resultnum = 0;
    u->buf = (char*)alloc( ud, NULL, 0, maxsize );
    assert(u->buf);
    u->used = 0;
    u->size = maxsize;
}
#define elementof(x) ( sizeof(x) / sizeof(x[0]))
inline void mpstackent_init( mpstackent_t *e, MP_CONTAINER_TYPE t, size_t expect ) {
    e->t = t;
    e->expect = expect;
    e->sofar = 0;
    e->sizesize = MP_CONTAINER_TYPE_sizesize(t);
    e->sizesofar = 0;
}



// move 1 byte forward
inline int unpacker_progress( unpacker_t *u, char ch ) {
    int i;
    for(i = u->nstacked-1; i >= 0; i-- ) {
        mpstackent_t *e = & u->stack[ i ];
        if( e->sizesofar < e->sizesize ) {
            MPDEBUGPRINT( "(%s Size:%d/%d)", MP_CONTAINER_TYPE_to_s(e->t), (int)e->sizesofar, (int)e->sizesize );
            e->sizebytes[ e->sizesofar ] = ch;
            e->sizesofar ++;
            if(e->sizesofar == e->sizesize){
                if( e->sizesize == 2 ){
                    e->expect = ntohs( *((short*)( e->sizebytes )) );
                    if( MP_CONTAINER_TYPE_is_map(e->t) ) e->expect *= 2;
                    MPDEBUGPRINT( "expect:%d ", (int)e->expect );
                } else if(e->sizesize == 4 ){
                    e->expect = ntohl( *((int*)( e->sizebytes )) );
                    if( MP_CONTAINER_TYPE_is_map(e->t) ) e->expect *= 2;                    
                    MPDEBUGPRINT( "expect:%d ", (int)e->expect );                    
                } else {
                    assert(!"possible bug in msgpack, not parse error" );
                }
            }
            break;
        } 
        e->sofar++;
        MPDEBUGPRINT( "(%s %d/%d)", MP_CONTAINER_TYPE_to_s(e->t),(int)e->sofar,(int)e->expect) ;
        //        assert( e->sofar <= e->expect );
        if(e->sofar < e->expect){
            break;
        }
        u->nstacked --;
        assert(u->nstacked >= 0);
        MPDEBUGPRINT( "fill-pop! " );
    }
    MPDEBUGPRINT( "\n");
    if( u->nstacked == 0 ) {
        MPDEBUGPRINT( "got result!\n" );
        u->resultnum ++;
        return 1;
    }
    return 0;
}
inline mpstackent_t *unpacker_top( unpacker_t *u ) {
    if( u->nstacked == 0 ) {
        return NULL;
    } else {
        return & u->stack[ u->nstacked - 1 ];
    }
}


inline void unpacker_progress_datasize( unpacker_t *u, size_t progress) {
    MPDEBUGPRINT( ">%d>",(int)progress);
    mpstackent_t *top = unpacker_top(u);
    size_t need = top->expect - top->sofar;
    assert( progress <= need );
    top->sofar += progress; 
    if( top->sofar == top->expect ){
        top->sofar -= 1; // sofar++ is in unpacker_progress!
        MPDEBUGPRINT("[DS]");
        unpacker_progress(u,0);
    }
}


// error:-1
// give expect=0 when need size bytes.
inline int unpacker_push( unpacker_t *u, MP_CONTAINER_TYPE t, int expect ) {
    MPDEBUGPRINT( "push: t:%s expect:%d\n", MP_CONTAINER_TYPE_to_s(t), expect );
    if( u->nstacked >= elementof(u->stack)) return -1;
    mpstackent_t *ent = & u->stack[ u->nstacked ];
    mpstackent_init( ent, t, expect );
    u->nstacked ++;
    // handle empty container
    if( MP_CONTAINER_TYPE_sizesize(t)==0 && expect == 0 ){
        MPDEBUGPRINT("[PUSH]");        
        unpacker_progress(u, 0x0);
    }
    return 0;
}


// need bytes, not values
inline int unpacker_container_needs_bytes( unpacker_t *u, size_t *dataneed ) {
    mpstackent_t *top = unpacker_top(u);
    if(!top)return 0;
    if( top->sizesofar < top->sizesize ) {
        *dataneed = 0;
        return 1;
    }
    
    if( top->sofar < top->expect ) {
        if( MP_CONTAINER_TYPE_is_container(top->t) ){
            return 0;
        } else {
            *dataneed = top->expect - top->sofar;
            return 1;
        }
    }
    return 0;
}




void unpacker_dump( unpacker_t *u ) {
    fprintf(stderr, "\n------\nnstacked:%d resultnum:%d\n", (int) u->nstacked, (int) u->resultnum );
    int i;
    for(i=0;i<(int)u->nstacked;i++){
        fprintf(stderr, "  stack[%d]: %s %d/%d\n", i, MP_CONTAINER_TYPE_to_s(u->stack[i].t), (int)u->stack[i].sofar, (int)u->stack[i].expect );
    }
    fprintf(stderr, "--------\n");
}
                   

                  
int unpacker_feed( unpacker_t *u, char *p, size_t len ) {

    if(u->used + len > u->size ){
        return -1;
    }
    memcpy( u->buf + u->used, p, len );
    u->used += len;


    
    size_t i;
    for(i=0;i<len;i++){
        unsigned char ch = (unsigned char)( p[i] );

#if MP_ENABLE_DEBUGPRINT
        int k;
        for(k=0;k<u->nstacked;k++){ MPDEBUGPRINT(" "); }
#endif        
        MPDEBUGPRINT( "[%x]:", ch );

        size_t dataneed; // for data, not for sizebytes.
        if( unpacker_container_needs_bytes(u, &dataneed ) ){
            if(dataneed>0){
                size_t progress = len-i;
                if( progress > dataneed ) {
                    progress = dataneed;
                }
                MPDEBUGPRINT( "dneed:%d progress:%d len:%d ", (int) dataneed,(int)progress, (int)len );
                i += progress -1; // sofar++ in unpacker_progress!
                unpacker_progress_datasize( u, progress );
            } else {
                MPDEBUGPRINT("[FEED]");
                unpacker_progress(u,ch);
            }
            continue;
        }
            
        if( ch <= 0x7f ){ // posfixnum
            unpacker_progress(u,ch);
        } else if( ch>=0x80 && ch<=0x8f ){ // fixmap
            int n = ch & 0xf;
            if(unpacker_push( u, MPCT_FIXMAP, n*2 )<0) return -1;
        } else if( ch>=0x90 && ch<=0x9f ){ // fixarray
            int n = ch & 0xf;
            if(unpacker_push( u, MPCT_FIXARRAY, n )<0) return -1;
        } else if( ch>=0xa0 && ch<=0xbf ){ // fixraw
            int n = ch & 0x1f;
            if(unpacker_push( u, MPCT_RAW, n )<0) return -1;
        } else if( ch==0xc0){ // nil
            unpacker_progress(u,ch);            
        } else if( ch==0xc1){ // reserved
            return -1;
        } else if( ch==0xc2){ // false
            unpacker_progress(u,ch);            
        } else if( ch==0xc3){ // true
            unpacker_progress(u,ch);            
        } else if( ch>=0xc4 && ch<=0xc9){ // reserved
            return -1;
        } else if( ch==0xca){ // float (4byte)
            if(unpacker_push( u, MPCT_FLOAT, 4 )<0) return -1;
        } else if( ch==0xcb){ // double (8byte)
            if(unpacker_push( u, MPCT_DOUBLE, 8 )<0) return -1;
        } else if( ch==0xcc){ // uint8 (1byte)
            if(unpacker_push( u, MPCT_UINT8, 1 )<0) return -1;
        } else if( ch==0xcd){ // uint16 (2byte)
            if(unpacker_push( u, MPCT_UINT16, 2 )<0) return -1;
        } else if( ch ==0xce){ // uint32
            if(unpacker_push( u, MPCT_UINT32, 4 )<0) return -1;
        } else if( ch ==0xcf){ // uint64
            if(unpacker_push( u, MPCT_UINT64, 8 )<0) return -1;
        } else if( ch ==0xd0){ // int8
            if(unpacker_push( u, MPCT_INT8, 1 )<0) return -1;
        } else if( ch == 0xd1){ // int16
            if(unpacker_push( u, MPCT_INT16, 2 )<0) return -1;
        } else if( ch == 0xd2){ // int32
            if(unpacker_push( u, MPCT_INT32, 4 )<0) return -1;            
        } else if( ch == 0xd3){ // int64
            if(unpacker_push( u, MPCT_INT64, 8 )<0) return -1;
        } else if( ch >= 0xd4 && ch <= 0xd9 ) { // reserved
            return -1;
        } else if( ch == 0xda){ // raw 16
            if(unpacker_push( u, MPCT_RAW16, 0 )<0) return -1;            
        } else if( ch == 0xdb){ // raw 32
            if(unpacker_push( u, MPCT_RAW32, 0 )<0) return -1;            
        } else if( ch == 0xdc){ // array 16
            if(unpacker_push( u, MPCT_ARRAY16, 0)<0) return -1;
        } else if( ch == 0xdd){ // array 32
            if(unpacker_push( u, MPCT_ARRAY32, 0)<0) return -1;
        } else if( ch == 0xde){ // map16
            if(unpacker_push( u, MPCT_MAP16, 0)<0) return -1;
        } else if( ch == 0xdf){ // map32
            if(unpacker_push( u, MPCT_MAP32, 0)<0) return -1;
        } else if( ch >= 0xe0 ) { // neg fixnum
            unpacker_progress(u,ch);
        }
            
    }
    return 0;                
}
void unpacker_shift( unpacker_t *u, size_t l ) {
    assert( l <= u->used );
    memmove( u->buf, u->buf + l, u->used - l );
    u->used -= l;
}

static int msgpack_unpacker_feed_api( lua_State *L ) {
    unpacker_t *u =  (unpacker_t*)luaL_checkudata( L, 1, "msgpack_unpacker" );
    size_t slen;
    const char *sval = luaL_checklstring(L, 2, &slen );
    MPDEBUGPRINT( "feed. used:%d len:%d\n",(int)u->used, (int)slen );
    int res = unpacker_feed( u, (char*)sval, slen );
    lua_pushnumber(L,res);
    return 1;
}
static int msgpack_unpacker_pull_api( lua_State *L ) {
    unpacker_t *u =  (unpacker_t*)luaL_checkudata( L, 1, "msgpack_unpacker" );
    MPDEBUGPRINT( "pull:%d\n", (int)u->resultnum );
    if( u->resultnum == 0 ) {
        lua_pushnil(L);
    } else {
        mprbuf_t rb;
        mprbuf_init( &rb, (const unsigned char*) u->buf, u->used );
        mprbuf_unpack_anytype(&rb,L);
        unpacker_shift( u, rb.ofs );
        u->resultnum --;
    }
    return 1;
}
static int msgpack_unpacker_gc_api( lua_State *L ) {
    void* ud;
    lua_Alloc alloc = lua_getallocf( L, &ud );
    unpacker_t *u =  (unpacker_t*)luaL_checkudata( L, 1, "msgpack_unpacker" );
    (void)alloc( ud, u->buf, 0, 0 );    // freeing
    u->buf = NULL;
    return 0;
}

typedef luaL_Reg luaL_reg;

static const luaL_reg msgpack_unpacker_m[] = {
    {"feed", msgpack_unpacker_feed_api },
    {"pull", msgpack_unpacker_pull_api },
    {"__gc", msgpack_unpacker_gc_api },
    {NULL,NULL}
};
    
static int msgpack_createUnpacker_api( lua_State *L ) {
    size_t bufsz = luaL_checknumber(L,1);
#if 0    
    //    { aho=7, hoge = { 5,6,"7", {8,9,10} }, fuga="11" }
    char data[28] = { 0x83, 0xa3, 0x61, 0x68,
                      0x6f, 0x7, 0xa4, 0x66,
                      0x75, 0x67, 0x61, 0xa2,
                      0x31, 0x31, 0xa4, 0x68,
                      0x6f, 0x67, 0x65, 0x94,
                      0x5, 0x6, 0xa1, 0x37,
                      0x93, 0x8, 0x9, 0xa };
#endif    
    unpacker_t *u = (unpacker_t*) lua_newuserdata( L, sizeof(unpacker_t));

    unpacker_init(L, u, bufsz );
    luaL_getmetatable(L, "msgpack_unpacker" );
    lua_setmetatable(L, -2 );
    return 1;
}


static int msgpack_largetbl( lua_State *L ) {
    int n = luaL_checkint(L,1);
    lua_createtable(L,n,0);
    int i;
    for(i=0;i<n;i++){
        lua_pushnumber(L,i);
        lua_rawseti(L,-2,i+1);
    }
    return 1;
}


static const luaL_reg msgpack_f[] = {
    {"pack", msgpack_pack_api },
    {"unpack", msgpack_unpack_api },
    {"createUnpacker", msgpack_createUnpacker_api },
    {"largetbl", msgpack_largetbl },
    { "feed", msgpack_unpacker_feed_api },
    { "pull", msgpack_unpacker_pull_api },
    { "proto", msgpack_proto_api },
    { "protoID", msgpack_protoID_api },
    { "encode", msgpack_encode_api },
    { "decode", msgpack_decode_api },
    {NULL,NULL}
};


LUALIB_API int luaopen_eproto_cpp ( lua_State *L ) {
	if( g_protos.size() == 0 ){
		g_protos.resize(1, NULL);	// 0下标不使用
		pthread_mutex_init(&g_mutex, NULL);
		pthread_mutex_init(&g_proto_mutex, NULL);
	}

    luaL_newmetatable(L, "msgpack_unpacker" );
    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");
	luaL_openlib(L, "unpacker", msgpack_unpacker_m, 0);

    luaL_openlib(L, "eproto_cpp", msgpack_f, 0);

    return 1;
}
