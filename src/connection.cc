#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netdb.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <cinttypes>

#include "connection.h"
#include "query.h"
#include "json_p.h"
#include "exceptions.h"
#include "term.h"

#include "rapidjson-config.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/encodedstream.h"
#include "rapidjson/document.h"

namespace RethinkDB
{

using QueryType = Protocol::Query::QueryType;

// constants
const int debug_net = 0;
Connection::Connection() {}
Connection::~Connection() {}

//My implementation of run_query
std::string Connection::serialize_query(const Query &query)
{
    auto serializable = query;
    return serializable.serialize();
}

std::string Connection::start_query(Term *term, uint64_t token, OptArgs &&opts)
{
    bool no_reply = false;
    auto it = opts.find("norelpy");
    if (it != opts.end())
    {
        no_reply = *(it->second.datum.get_boolean());
    }
    return serialize_query(Query{QueryType::START, token, term->datum, std::move(opts)});
}

std::string Connection::stop_query(uint64_t token)
{
    return serialize_query(Query{QueryType::STOP, token});
}
std::string Connection::continue_query(uint64_t token)
{
    return serialize_query(Query{QueryType::CONTINUE, token});
}

Error Response::as_error()
{
    std::string repr;
    if (result.size() == 1)
    {
        std::string *string = result[0].get_string();
        if (string)
        {
            repr = *string;
        }
        else
        {
            repr = write_datum(result[0]);
        }
    }
    else
    {
        repr = write_datum(Datum(result));
    }
    std::string err;
    using RT = Protocol::Response::ResponseType;
    using ET = Protocol::Response::ErrorType;
    switch (type)
    {
    case RT::SUCCESS_SEQUENCE:
        err = "unexpected response: SUCCESS_SEQUENCE";
        break;
    case RT::SUCCESS_PARTIAL:
        err = "unexpected response: SUCCESS_PARTIAL";
        break;
    case RT::SUCCESS_ATOM:
        err = "unexpected response: SUCCESS_ATOM";
        break;
    case RT::WAIT_COMPLETE:
        err = "unexpected response: WAIT_COMPLETE";
        break;
    case RT::SERVER_INFO:
        err = "unexpected response: SERVER_INFO";
        break;
    case RT::CLIENT_ERROR:
        err = "ReqlDriverError";
        break;
    case RT::COMPILE_ERROR:
        err = "ReqlCompileError";
        break;
    case RT::RUNTIME_ERROR:
        switch (error_type)
        {
        case ET::INTERNAL:
            err = "ReqlInternalError";
            break;
        case ET::RESOURCE_LIMIT:
            err = "ReqlResourceLimitError";
            break;
        case ET::QUERY_LOGIC:
            err = "ReqlQueryLogicError";
            break;
        case ET::NON_EXISTENCE:
            err = "ReqlNonExistenceError";
            break;
        case ET::OP_FAILED:
            err = "ReqlOpFailedError";
            break;
        case ET::OP_INDETERMINATE:
            err = "ReqlOpIndeterminateError";
            break;
        case ET::USER:
            err = "ReqlUserError";
            break;
        case ET::PERMISSION_ERROR:
            err = "ReqlPermissionError";
            break;
        default:
            err = "ReqlRuntimeError";
            break;
        }
    }
    throw Error("%s: %s", err.c_str(), repr.c_str());
}

Protocol::Response::ResponseType response_type(double t)
{
    int n = static_cast<int>(t);
    using RT = Protocol::Response::ResponseType;
    switch (n)
    {
    case static_cast<int>(RT::SUCCESS_ATOM):
        return RT::SUCCESS_ATOM;
    case static_cast<int>(RT::SUCCESS_SEQUENCE):
        return RT::SUCCESS_SEQUENCE;
    case static_cast<int>(RT::SUCCESS_PARTIAL):
        return RT::SUCCESS_PARTIAL;
    case static_cast<int>(RT::WAIT_COMPLETE):
        return RT::WAIT_COMPLETE;
    case static_cast<int>(RT::CLIENT_ERROR):
        return RT::CLIENT_ERROR;
    case static_cast<int>(RT::COMPILE_ERROR):
        return RT::COMPILE_ERROR;
    case static_cast<int>(RT::RUNTIME_ERROR):
        return RT::RUNTIME_ERROR;
    default:
        throw Error("Unknown response type");
    }
}

Protocol::Response::ErrorType runtime_error_type(double t)
{
    int n = static_cast<int>(t);
    using ET = Protocol::Response::ErrorType;
    switch (n)
    {
    case static_cast<int>(ET::INTERNAL):
        return ET::INTERNAL;
    case static_cast<int>(ET::RESOURCE_LIMIT):
        return ET::RESOURCE_LIMIT;
    case static_cast<int>(ET::QUERY_LOGIC):
        return ET::QUERY_LOGIC;
    case static_cast<int>(ET::NON_EXISTENCE):
        return ET::NON_EXISTENCE;
    case static_cast<int>(ET::OP_FAILED):
        return ET::OP_FAILED;
    case static_cast<int>(ET::OP_INDETERMINATE):
        return ET::OP_INDETERMINATE;
    case static_cast<int>(ET::USER):
        return ET::USER;
    default:
        throw Error("Unknown error type");
    }
}

} // namespace RethinkDB
