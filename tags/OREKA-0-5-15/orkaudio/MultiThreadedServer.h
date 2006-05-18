/*
 * Oreka -- A media capture and retrieval platform
 * 
 * Copyright (C) 2005, orecx LLC
 *
 * http://www.orecx.com
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License.
 * Please refer to http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef __MULTITHREADEDSERVER_H__
#define __MULTITHREADEDSERVER_H__

#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"

/** This server accepts permanent telnet like connections.
	commands are accepted in "single line" format.
	one thread per connection
*/
class CommandLineServer : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:
	virtual int open (void *);
	/** daemon thread */
	static void run(void *args);
	/** service routine */
	virtual int svc (void);
};
typedef ACE_Acceptor<CommandLineServer, ACE_SOCK_ACCEPTOR> CommandLineAcceptor;


/** This server is a lightweight http server that extracts commands from URLs and outputs results in xml format
	one thread per connection
	Example url:
	http://localhost:23000/message=print&text=hello
*/
class HttpServer : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:
	virtual int open (void *);
	/** daemon thread */
	static void run(void *args);
	/** service routine */
	virtual int svc (void);
};
typedef ACE_Acceptor<HttpServer, ACE_SOCK_ACCEPTOR> HttpAcceptor;

#endif

