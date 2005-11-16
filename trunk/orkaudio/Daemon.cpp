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

#define _WINSOCKAPI_		// prevents the inclusion of winsock.h

#include "Utils.h"
#ifdef WIN32
#include <windows.h>
#include <tchar.h>
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;
#endif
#include "Daemon.h"

#ifdef WIN32
void WINAPI ServiceControlHandler( DWORD controlCode )
{
	switch ( controlCode )
	{
		case SERVICE_CONTROL_INTERROGATE:
			break;

		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus( serviceStatusHandle, &serviceStatus );
			DaemonSingleton::instance()->Stop();
			return;

		case SERVICE_CONTROL_PAUSE:
			break;

		case SERVICE_CONTROL_CONTINUE:
			break;

		default:
			;
	}

	SetServiceStatus( serviceStatusHandle, &serviceStatus );
}
#endif

void Daemon::Initialize(CStdString serviceName, DaemonHandler runHandler, DaemonHandler stopHandler)
{
	m_runHandler = runHandler;
	m_stopHandler = stopHandler;
	m_serviceName = serviceName;

	m_stopping = false;
}

void Daemon::Start()
{
#ifdef WIN32
	// change current directory to service location (default for NT services is system32)
	CStdString workingDirectory;

	TCHAR path[ _MAX_PATH + 1 ];
	if ( GetModuleFileName( 0, path, sizeof(path)/sizeof(path[0]) ) > 0 )
	{
		CStdString pathString = path;
		int lastBackSlashPosition = pathString.ReverseFind("\\");
		if (lastBackSlashPosition != -1)
		{
			workingDirectory = pathString.Left(lastBackSlashPosition);
			chdir((PCSTR)workingDirectory);
		}
	}

	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ (char *)(PCSTR)m_serviceName, Daemon::Run },
		{ 0, 0 }
	};

	StartServiceCtrlDispatcher( serviceTable );
#else
	Daemon::Run();
#endif
}

#ifdef WIN32
void WINAPI Daemon::Run( DWORD /*argc*/, TCHAR* /*argv*/[] )
#else
void Daemon::Run()
#endif
{
#ifdef WIN32
	// initialise service status
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler( (PCSTR)DaemonSingleton::instance()->m_serviceName, ServiceControlHandler );
	if ( serviceStatusHandle )
	{
		// service is starting
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		// running
		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );
	}
#else	// non WIN32
        int i,lfp;
        char str[10];
        if(getppid()==1) return; /* already a daemon */
        i=fork();
	if (i<0) exit(1); /* fork error */
	if (i>0) exit(0); /* parent exits */
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	//chdir(RUNNING_DIR); /* change running directory */
	//lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	//if (lfp<0) exit(1); /* can not open */
	//if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	//sprintf(str,"%d\n",getpid());
	//write(lfp,str,strlen(str)); /* record pid to lockfile */
	//signal(SIGCHLD,SIG_IGN); /* ignore child */
	//signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	//signal(SIGTTOU,SIG_IGN);
	//signal(SIGTTIN,SIG_IGN);
	//signal(SIGHUP,signal_handler); /* catch hangup signal */
#endif

	DaemonSingleton::instance()->m_runHandler();

#ifdef WIN32
	// service was stopped
	serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );

	// do cleanup here
	CloseHandle( stopServiceEvent );
	stopServiceEvent = 0;

	// service is now stopped
	serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );
#endif
}

void Daemon::Stop()
{
	m_stopping = true;
	m_stopHandler();
}

void Daemon::Install()
{
#ifdef WIN32
	SC_HANDLE serviceControlManager = OpenSCManager( 0, 0, SC_MANAGER_CREATE_SERVICE );

	if ( serviceControlManager )
	{
		TCHAR path[ _MAX_PATH + 1 ];
		if ( GetModuleFileName( 0, path, sizeof(path)/sizeof(path[0]) ) > 0 )
		{
			SC_HANDLE service = CreateService( serviceControlManager,
							(PCSTR)m_serviceName, (PCSTR)m_serviceName,
							SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
							SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path,
							0, 0, 0, 0, 0 );
			if ( service )
				CloseServiceHandle( service );
		}

		CloseServiceHandle( serviceControlManager );
	}
#endif
}

void Daemon::Uninstall()
{
#ifdef WIN32
	SC_HANDLE serviceControlManager = OpenSCManager( 0, 0, SC_MANAGER_CONNECT );

	if ( serviceControlManager )
	{
		SC_HANDLE service = OpenService( serviceControlManager,
			(PCSTR)m_serviceName, SERVICE_QUERY_STATUS | DELETE );
		if ( service )
		{
			SERVICE_STATUS serviceStatus;
			if ( QueryServiceStatus( service, &serviceStatus ) )
			{
				if ( serviceStatus.dwCurrentState == SERVICE_STOPPED )
					DeleteService( service );
			}

			CloseServiceHandle( service );
		}

		CloseServiceHandle( serviceControlManager );
	}
#endif
}

bool Daemon::IsStopping()
{
	return m_stopping;
}

