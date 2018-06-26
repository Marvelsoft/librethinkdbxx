#ifndef QUERY_H_
#define QUERY_H_

#include <inttypes.h>

#include "connection.h"
#include "json_p.h"
#include "datum.h"

#include "term.h"
namespace RethinkDB
{

extern const int debug_net;
struct Query
{
    Protocol::Query::QueryType type;
    uint64_t token;
    Datum term;
    std::map<std::string, Term> optArgs;

    std::string serialize()
    {
        Array query_arr{static_cast<double>(type)};
        if (term.is_valid())
            query_arr.emplace_back(term);
        if (!optArgs.empty())
            query_arr.emplace_back(Term(std::move(optArgs)).datum);

        std::string query_str = write_datum(query_arr);
        if (debug_net > 0)
        {
            fprintf(stderr, "[%" PRIu64 "] >> %s\n", token, query_str.c_str());
        }

        char header[12];
        memcpy(header, &token, 8);
        uint32_t size = query_str.size();
        memcpy(header + 8, &size, 4);
        query_str.insert(0, header, 12);
        return query_str;
    }
};

// Used internally to convert a raw response type into an enum
Protocol::Response::ResponseType response_type(double t);
Protocol::Response::ErrorType runtime_error_type(double t);

// Contains a response from the server. Use the Cursor class to interact with these responses
class Response
{
  public:
    Response() = delete;
    explicit Response(Datum &&datum) : type(response_type(std::move(datum).extract_field("t").extract_number())),
                                       error_type(datum.get_field("e") ? runtime_error_type(std::move(datum).extract_field("e").extract_number()) : Protocol::Response::ErrorType(0)),
                                       result(std::move(datum).extract_field("r").extract_array()) {}
    Error as_error();
    Protocol::Response::ResponseType type;
    Protocol::Response::ErrorType error_type;
    Array result;
};

} // namespace RethinkDB

#endif // QUERY_H_
