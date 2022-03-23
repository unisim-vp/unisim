/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA),
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#include <unisim/util/dbgate/dbgate.hh>
#include <limits>
#include <iostream>

namespace unisim {
namespace util {
namespace dbgate {
  
  DBGated::DBGated(int port, char const* _root)
    : hypapp::HttpServer(port ? port : 12345, 16)
    , root()
  {
    if (_root)
      root.assign(_root);
    else
      { char tmpdirbuf[] = "/tmp/dbgateXXXXXX"; root = mkdtemp( tmpdirbuf ); }
    StartLoopThread();
  }

  DBGated::~DBGated()
  {
    JoinLoopThread();
  }
   

  
  void
  DBGated::write(int cd, char const* buffer, uintptr_t size)
  {
    auto itr = ostreams.find(cd);
    if (itr != ostreams.end())
      itr->second.stream.write(buffer,size);
  }
  
  int
  DBGated::open(char const* path)
  {
    std::string filepath = root + "/dbg";
    {
      /* generating unique name with file count */
      struct { void nibble(std::string& s, uintptr_t i) { if (i>=16) nibble(s,i>>4); s += "0123456789abcdef"[i%16]; } } _;
      _.nibble( filepath, files.size() + ostreams.size() );
      /* extracting any existing extension */
      std::string buffer = path;
      uintptr_t spos = buffer.rfind('/'), dpos = buffer.rfind('.');
      if (dpos != std::string::npos and (spos == std::string::npos or spos < dpos))
        filepath.insert(filepath.end(),buffer.begin()+dpos,buffer.end());
    }
    
    int rcd = 0;
    auto itr = ostreams.end();
    
    if (ostreams.size())
      {
        rcd = std::numeric_limits<int>::max();
        for (auto end = itr; --itr != end;)
          {
            if (itr->first < rcd) { rcd = itr->first+1; break; }
            else                  { rcd = itr->first-1; continue; }
          }
      }
    
    if (rcd < 0)
      {
        std::cerr << "Descriptor allocation error\n";
        return rcd;
      }
    
    itr = ostreams.emplace_hint( itr, std::piecewise_construct, std::forward_as_tuple( rcd ), std::forward_as_tuple( path, std::move(filepath) ) );
    
    if (not itr->second.stream.good())
      {
        std::cerr << "Can't create Sink(" << itr->second.chanpath << ", " << itr->second.filepath << ")\n";
        ostreams.erase(itr);
        return -1;
      }

    return rcd;
  }

  void
  DBGated::close(int cd)
  {
    auto itr = ostreams.find(cd);
    if (itr == ostreams.end())
      return;
    Sink& sink(itr->second);
    sink.stream.close();
    files.emplace( std::piecewise_construct, std::forward_as_tuple( std::move(sink.chanpath) ), std::forward_as_tuple( std::move(sink.filepath)) );
    ostreams.erase(itr);
  }

  DBGated::Sink::Sink(std::string&& _chanpath, std::string&& _filepath)
    : chanpath(std::move(_chanpath)), filepath(std::move(_filepath)), stream(filepath.c_str())
  {}

  namespace
  {
    template <uintptr_t N>
    char const*
    pop( std::string const& str, char const (&ref)[N] )
    {
      for (uintptr_t idx = 0; idx < (N-1); ++idx)
        if (ref[idx] != str[idx]) return 0;
      return &str[N-1];
    }

    char const*
    pop( std::string const& str, std::string const& ref )
    {
      if (str.compare(0,ref.size(),ref) != 0)
        {
          std::cerr << "Can't pop " << ref << " from " << str << "\n";
          return str.c_str();
        }
      return &str[ref.size()];
    }
  }
  
  void
  DBGated::Serve(hypapp::ClientConnection const& conn)
  {
    struct MessageLoop : public unisim::util::hypapp::MessageLoop
    {
      MessageLoop(DBGated& _http_server)
        : unisim::util::hypapp::MessageLoop(_http_server, std::cerr, std::cerr, std::cerr)
        , http_server(_http_server)
      {
      }
		
      virtual bool SendResponse(unisim::util::hypapp::Request const& req, unisim::util::hypapp::ClientConnection const& conn)
      {
        unisim::util::hypapp::HttpRequest http_request(req, std::cerr, std::cerr, std::cerr);
			
        if(http_request.IsValid()) // http request is valid?
          {
            std::cerr << "URI \"" << req.GetRequestURI() << "\" is valid\n";
            

            if (char const* filename = pop(http_request.GetAbsolutePath(), "/get/"))
              {
                std::cerr << "URI \"" << req.GetRequestURI() << "\" refers to file\n";
                return http_server.ServeFile(http_request, conn, filename);
              }
            
            //   if(http_request.GetAbsolutePath() == "/favicon.ico")
            //     {
            //       return http_server.ServeFile(http_request, http_server.GetSimulator()->GetSharedDataDirectory() + "/unisim/service/http_server/favicon.ico", conn);
            //     }
            
            if (char const* filename = pop(http_request.GetAbsolutePath(), "/"))
              {
                std::cerr << "URI \"" << req.GetRequestURI() << "\" refers to root\n";
                return http_server.ServeView(http_request, conn, filename);
              }
          }
        else
          {
            std::cerr << "URI \"" << req.GetRequestURI() << "\" is invalid\n";
          }
			
        std::cerr << "sending HTTP response 404 Not Found\n";
			
        if (not http_server.ServeError(http_request, conn, hypapp::HttpResponse::NOT_FOUND))
          {
            std::cerr << "I/O error or connection closed by peer while sending HTTP 404 Not Found\n";
          }

        return false;
      }
		
    private:
      DBGated& http_server;
    };
	
    MessageLoop msg_loop(*this);
    msg_loop.Run(conn);
  }

  bool
  DBGated::ServeView(hypapp::HttpRequest const& req, hypapp::ClientConnection const& conn, char const* filename)
  {
    hypapp::HttpResponse response;
    
    response << "<!DOCTYPE html>\n<html lang=\"en\">\n";
    response << "<head><title>Debug Visual (" << filename << ")</title>\n"
             << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
             << "<meta name=\"description\" content=\"Debug Visual (" << filename << ")\">"
             << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
             << "<style>.selected {background-color: #FFFF00;}</style>"
             << "</head>\n";
    response << "<body>";
    response << "<table><tbody>\n"
             << "<tr><td rowspan=\"2\"><ul>\n";

    std::string filepath = root + "/" + filename;

    typedef decltype(&*files.cbegin()) namepair;
    namepair last = 0, prev = 0, selected = 0, next = 0;
    for (auto const& kv : files)
      {
        bool this_one = kv.second == filepath;
        if (this_one)
          { prev = last; selected = &kv; }
        else if (selected == last)
          { next = &kv; }
        response << "<li><a " << (this_one ? " class=\"selected\"" : "") << " href=\"" << pop(kv.second, root) << "\">" << kv.first << "</a></li>\n";
        last = &kv;
      }
    
    response << "</ul></td>\n";
    response << "<td>";
    if (prev) response << "<a href=\"" << pop(prev->second, root) << "\">&larr;</a>";
    if (next) response << "<a href=\"" << pop(next->second, root) << "\">&rarr;</a>";
    response << "</td></tr>\n";
    response << "<tr><td><iframe src=\"/get/" << filename << "\" title=\"Debug Object View\"></iframe></td></tr>\n";
    response << "</tbody></table>\n";
    response << "</body>\n";
    response << "</html>\n";

    return conn.Send(response.ToString());
  }
  
  bool
  DBGated::ServeFile(hypapp::HttpRequest const& req, hypapp::ClientConnection const& conn, char const* filename)
  {
    hypapp::HttpResponse response;

    std::string filepath = root + "/" + filename;
    
    std::ifstream file(filepath, std::fstream::binary);

    if (file.is_open())
      {
        if ((req.GetRequestType() == unisim::util::hypapp::Request::GET) or (req.GetRequestType() == unisim::util::hypapp::Request::HEAD))
          {
            if (file.seekg(0, file.end))
              {
                std::streampos length = file.tellg();
				
                if(file.seekg(0, file.beg))
                  {
                    // if(enable_cache)
                    //   {
                    //     response.EnableCache();
                    //   }

                    
                    std::string ext = std::string(filename);
                    ext = ext.substr(ext.find_last_of("/."));
					
                    if((ext == ".htm") || (ext == ".html"))
                      response.SetContentType("text/html; charset=utf-8");
                    else if(ext == ".css")
                      response.SetContentType("text/css");
                    else if(ext == ".js")
                      response.SetContentType("application/javascript");
                    else if(ext == ".png")
                      response.SetContentType("image/png");
                    else if((ext == ".jpg") || (ext == ".jpeg"))
                      response.SetContentType("image/jpeg");
                    else if(ext == ".svg")
                      response.SetContentType("image/svg+xml");
                    else if(ext == ".ico")
                      response.SetContentType("image/x-icon");
                    else
                      response.SetContentType("application/octet-stream");

                    if(length > 0)
                      {
                        char buffer[4096];
                        std::streampos count = length;
                        do
                          {
                            std::streamsize n = file.readsome(buffer, sizeof(buffer));
							
                            if(file.fail()) return false;
							
                            if(n > 0)
                              {
                                response.write(buffer, n);
                                count -= n;
                              }
                          }
                        while(count > 0);
                      }
                  }
                else
                  {
                    std::cerr << "Can't seek at beginning of File \"" << filepath << "\"\n";
                    return ServeError(req, conn, hypapp::HttpResponse::INTERNAL_SERVER_ERROR);
                  }
              }
            else
              {
                std::cerr << "Can't seek at end of File \"" << filepath << "\"\n";
                return ServeError(req, conn, hypapp::HttpResponse::INTERNAL_SERVER_ERROR);
              }
          }
        else if(req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)
          {
            response.Allow("OPTIONS, GET, HEAD");
          }
        else
          {
            std::cerr << "Method not allowed\n";
            return Serve405(req, conn, "OPTIONS, GET, HEAD");
          }
      }
    else
      {
        std::cerr << "Can' open File \"" << filepath << "\" for reading\n";
        return ServeError(req, conn, hypapp::HttpResponse::NOT_FOUND);
      }
	
    return conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));
  }
  
  bool DBGated::Serve405(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, char const* methods)
  {
    unisim::util::hypapp::HttpResponse response;
	
    response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
    response.Allow(methods);
	
    response << "<!DOCTYPE html>" << std::endl;
    response << "<html lang=\"en\">" << std::endl;
    response << "\t<head>" << std::endl;
    response << "\t\t<title>Error 405 (Method Not Allowed)</title>" << std::endl;
    response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
    response << "\t\t<meta name=\"description\" content=\"Error 405 (Method Not Allowed)\">" << std::endl;
    response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
    response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
    response << "\t\t<style>" << std::endl;
    response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
    response << "\t\t</style>" << std::endl;
    response << "\t</head>" << std::endl;
    response << "\t<body>" << std::endl;
    response << "\t\t<p>HTTP Method not allowed</p>" << std::endl;
    response << "\t</body>" << std::endl;
    response << "</html>" << std::endl;
	
    return conn.Send(response.ToString());
  }

  bool
  DBGated::ServeError(hypapp::HttpRequest const& req, hypapp::ClientConnection const& conn, hypapp::HttpResponse::StatusCode response_code)
  {
    hypapp::HttpResponse response;
    
    response.SetStatus(response_code);

    char const* msg = 0;
    switch (response_code)
      {
      case unisim::util::hypapp::HttpResponse::NOT_FOUND: msg = "Not Found"; break;
      default: msg = "???"; break;
      }
    

    response << "<!DOCTYPE html>\n<html lang=\"en\">\n";
    response << "<head><title>Error " << int(response_code) << " (" << msg << ")</title>\n"
             << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
             << "<meta name=\"description\" content=\"Error " << int(response_code) << " (" << msg << ")\">"
             << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
             << "</head>\n";
    response << "<body><p>Unavailable</p></body>\n";
    response << "</html>\n";

    return conn.Send(response.ToString());
  }
  
} /* end of namespace dbgate */
} /* end of namespace util */
} /* end of namespace unisim */

