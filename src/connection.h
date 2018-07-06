#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

#include "protocol_defs.h"
#include "datum.h"
#include "error.h"

#define FOREVER (-1)
#define SECOND 1
#define MICROSECOND 0.000001

namespace RethinkDB
{
class Term;
using OptArgs = std::map<std::string, Term>;
struct Query;
// A connection to a RethinkDB server
// It contains:
//  * A socket
//  * Read and write locks
//  * A cache of responses that have not been read by the corresponding Cursor
//class ConnectionPrivate;
  class Connection
{
public:
  Connection();
  Connection(const Connection &) noexcept = delete;
  Connection(Connection &&) noexcept = delete;
  Connection &operator=(Connection &&) noexcept = delete;
  Connection &operator=(const Connection &) noexcept = delete;
  ~Connection();

  void close();

  //explicit Connection(ConnectionPrivate *dd);
  //std::unique_ptr<ConnectionPrivate> d;
static std::string start_query(Term *term, uint64_t token, OptArgs &&args);
static std::string stop_query(uint64_t token);
static std::string continue_query(uint64_t token);
static std::string serialize_query(const Query &query);

  //My implementation of start,stop and continue query
private:
  friend class Token;
  friend class Term;
  friend std::unique_ptr<Connection>;
};

} // namespace RethinkDB
