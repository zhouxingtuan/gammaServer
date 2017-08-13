//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/27
// Time: 下午9:15
// To change this template use File | Settings | File Templates.
//

#include <lua.hpp>
#include <string>
#include <stdexcept>
#include <math.h>
#include <list>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"

using namespace rapidjson;
using namespace std;

struct DecodeHandle
{
	lua_State* L;
	const void* root;
	list<int> arr_count;
	DecodeHandle(lua_State* l)
		: L(l)
		, root(NULL)
	{}
	bool rawset()
	{
		int& nCount = arr_count.front();
		if (nCount == -1)
		{
			if (lua_type(L, -2) != LUA_TSTRING
				|| lua_type(L, -3) != LUA_TTABLE)

			{
				return false;
			}
			lua_rawset(L, -3);
		}
		else
		{
			if (lua_type(L, -2) != LUA_TTABLE)
			{
				return false;
			}
			lua_rawseti(L, -2, nCount);
			nCount++;
		}
		return true;
	}
	~DecodeHandle()
	{}
	bool Null()
	{
		lua_pushlightuserdata(L, NULL);
		return rawset();
	}
	bool Bool(bool b)
	{
		lua_pushboolean(L, b ? 1 : 0);
		return rawset();
	}
	bool Int(int i)
	{
		lua_pushinteger(L, i);
		return rawset();
	}
	bool Uint(unsigned u)
	{
		lua_pushinteger(L, u);
		return rawset();
	}
	bool Int64(int64_t i)
	{
		lua_pushinteger(L, i);
		return rawset();
	}
	bool Uint64(uint64_t u)
	{
		lua_pushinteger(L, u);
		return rawset();
	}
	bool Double(double d)
	{
		lua_pushnumber(L, d);
		return rawset();
	}
	bool RawNumber(const char* str, SizeType length, bool copy)
	{
		lua_pushlstring(L, str, length);
		return rawset();
	}
	bool String(const char* str, SizeType length, bool copy)
	{
		lua_pushlstring(L, str, length);
		return rawset();
	}
	bool StartObject()
	{
		lua_newtable(L);
		if (root == NULL)
		{
			root = lua_topointer(L, -1);
			arr_count.push_front(-1);
			return true;
		}
		int nCount = arr_count.front();
		if (nCount > 0)
		{
			rawset();
			lua_pushinteger(L, nCount);
			lua_rawget(L, -2);
		}
		else
		{
			size_t sz = 0;
			const char* pKey = lua_tolstring(L, -2, &sz);
			rawset();
			lua_pushlstring(L, pKey, sz);
			lua_rawget(L, -2);
		}

		arr_count.push_front(-1);

		return true;
	}
	bool Key(const char* str, SizeType length, bool copy)
	{
		lua_pushlstring(L, str, length);
		return true;
	}
	bool EndObject(SizeType memberCount)
	{
		if (( !( lua_type(L, -1) == LUA_TTABLE ) )
			|| ( lua_topointer(L, -1) != root ))
		{
			lua_pop(L, 1);
		}
		arr_count.pop_front();
		return true;
	}
	bool StartArray()
	{
		lua_newtable(L);
		if (root == NULL)
		{
			root = lua_topointer(L, -1);
			arr_count.push_front(1);
			return true;
		}

		int nCount = arr_count.front();
		if (nCount > 0)
		{
			rawset();
			lua_pushinteger(L, nCount);
			lua_rawget(L, -2);
		}
		else
		{
			size_t sz = 0;
			const char* pKey = lua_tolstring(L, -2, &sz);
			rawset();
			lua_pushlstring(L, pKey, sz);
			lua_rawget(L, -2);
		}
		arr_count.push_front(1);
		return true;
	}
	bool EndArray(SizeType elementCount)
	{
		if (( !( lua_type(L, -1) == LUA_TTABLE ) )
			|| ( lua_topointer(L, -1) != root ))
		{
			lua_pop(L, 1);
		}
		arr_count.pop_front();
		return true;
	}
};
static int decode(lua_State* L)
{
	if (lua_isstring(L, -1))
	{
		size_t sz = 0;
		const char*  json = lua_tolstring(L, -1, &sz);
		DecodeHandle handler(L);
		Reader reader;
		StringStream ss(json);
		if (!reader.Parse(ss, handler))
		{
			lua_pushnil(L);
		}
		return 1;
	}
	lua_pushnil(L);
	return 1;
}


static bool encodehelper(Writer<StringBuffer>& writer, list<void*>& pTables, lua_State* L);
static int lua_array_length(lua_State *L)
{
	double k;
	int max;
	max = 0;
	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		if (lua_type(L, -2) == LUA_TNUMBER &&
			( k = lua_tonumber(L, -2) ))
		{
			if (floor(k) == k && k >= 1)
			{
				if (k > max)
				{
					max = k;
				}
				lua_pop(L, 1);
				continue;
			}
		}
		lua_pop(L, 2);
		return -1;
	}
	return max;
}

static bool encode_array(Writer<StringBuffer>& writer, list<void*>& pTables, lua_State* L, int array_length)
{
	writer.StartArray();
	for (int i = 1; i <= array_length; i++)
	{
		lua_rawgeti(L, -1, i);
		if (!encodehelper(writer, pTables, L))
		{
			return false;
		}
	}
	writer.EndArray();
	lua_pop(L, 1);
	return true;
}

static bool encode_object(Writer<StringBuffer>& writer, list<void*>& pTables, lua_State* L)
{
	writer.StartObject();
	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		int keytype = lua_type(L, -2);
		if (keytype == LUA_TNUMBER)
		{
			writer.Key(std::to_string(lua_tointeger(L, -2)).c_str());
		}
		else if (keytype == LUA_TSTRING)
		{
			size_t sz;
			const char* pStr = lua_tolstring(L, -2, &sz);
			writer.Key(pStr, sz);
		}
		else
		{
			return false;
		}

		if (!encodehelper(writer, pTables, L))
		{
			return false;
		}
	}

	writer.EndObject();
	lua_pop(L, 1);
	return true;
}

static bool encodehelper(Writer<StringBuffer>& writer, list<void*>& pTables, lua_State* L)
{
	switch (lua_type(L, -1))
	{
		case LUA_TNIL:
			writer.Null();
			break;
		case LUA_TNUMBER:
		{
#if defined(LUA_VERSION_NUM) &&  LUA_VERSION_NUM >= 503
			if (lua_isinteger(L, -1))
			{
				writer.Int64(lua_tointeger(L, -1));
			}
			else
			{
				writer.Double(lua_tonumber(L, -1));
			}
#else
			LUA_INTEGER iValue = lua_tointeger(L, -1);
			LUA_NUMBER  numValue = lua_tonumber(L, -1);
			if (iValue != numValue)
			{
				writer.Double(numValue);
			}
			else
			{
				writer.Int64(iValue);
			}
#endif
		}
		break;
		case LUA_TBOOLEAN:
			writer.Bool(lua_toboolean(L, -1));
			break;
		case LUA_TSTRING:
		{
			size_t sz = 0;
			const char* pStr = lua_tolstring(L, -1, &sz);
			writer.String(pStr, sz);
			break;
		}
		case LUA_TTABLE:
		{
			const void *p = lua_topointer(L, -1);
			for (auto k : pTables)
			{
				if (k == p)
				{
					lua_pop(L, 1);
					return true;
				}
			}
			pTables.push_back((void*) p);
			int nMax = lua_array_length(L);
			if (nMax > 0)
			{
				return encode_array(writer, pTables, L, nMax);
			}
			else
			{
				return encode_object(writer, pTables, L);
			}
			break;
		}
		case LUA_TFUNCTION:
		{
			break;
		}
		case LUA_TUSERDATA:
		{
			break;
		}
		case LUA_TTHREAD:
		{
			break;
		}
		case LUA_TLIGHTUSERDATA:
		{
			void* p = lua_touserdata(L, -1);
			if (p == NULL)
			{
				writer.Null();
			}
			break;
		}
		default:
			return false;
	}
	lua_pop(L, 1);
	return true;
}


static int encode(lua_State* L)
{
	StringBuffer s(0, 1024 * 1024 * 10);
	Writer<StringBuffer> writer(s);
	list<void*> pTables;
	try
	{
		if (encodehelper(writer, pTables, L))
		{
			lua_pushlstring(L, s.GetString(), s.GetSize());
			return 1;
		}
	}
	catch (std::runtime_error& e)
	{

	}
	lua_pushnil(L);
	return 1;

}


extern "C" int luaopen_rjson(lua_State *L)
{
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 503
	luaL_checkversion(L);
#endif
	luaL_Reg l[] = {
		{ "encode", encode },
		{ "decode", decode },
		{ NULL, NULL },
	};
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 502
	luaL_newlib(L, l);
#else
//	luaL_register(L,"rjson",l);
	luaL_openlib(L, "rjson", l, 0);
#endif
	lua_pushlightuserdata(L, NULL);
	lua_setfield(L, -2, "null");
	return 1;
}
