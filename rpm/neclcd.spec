Name:		neclcd
Version:	0.1
Release:	2.%{dist}.cern
Summary:	Command line interface for RS-232 communication with the LCD screen NEC MultiSync LCD4610.
Packager:	Jiri Tyr

Group:		Productivity/Networking
License:	GPLv3
URL:		http://www.cern.ch
Source:		%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires:	gcc
BuildRequires:	make
BuildRequires:	glibc-devel
BuildRequires:	perl(ExtUtils::Manifest)


%description
Command line interface for RS-232 communication with the LCD screen NEC
MultiSync LCD4610. It allows to read and write any parameters of the screen
over the serial line connected to the screen.


%prep
%setup -q -n %{name}-%{version}


%build
%{__make} %{?_smp_mflags}


%install
[ "%{buildroot}" != / ] && %{__rm} -rf "%{buildroot}"
%{__make} install DESTDIR=%{buildroot}


%clean
[ "%{buildroot}" != / ] && %{__rm} -rf "%{buildroot}"


%files
%defattr(-,root,root,-)
# For license text(s), see the perl package.
%doc Changes README
%{_bindir}/neclcd


%changelog
* Wed May 6 2009 Jiri Tyr <jiri.tyr at cern.ch>
- First build.
