Name:		bestfit
Version:	0.2.0
Release:	1%{?dist}
Summary:	Optimally choose files to be put on a CD (or other media)

Group:	Other
License:	GPLv2
URL:		http://www.student.lu.se/~nbi98oli/
Source:  bestfit-0.2.0.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
Bestfit is a small program to determine which files that should be put
on a CD (or other media), so that as little space as possible is wasted.
It is very easy to use: you specify files on the command line, and bestfit
prints the names of those that were selected. Alternatively, bestfit
can execute a command for each selected file (e.g. to move them to a
different directory).

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_mandir}/man1/%{name}.1*

%changelog
* Mon Apr 23 2001 Oskar Liljeblad <oskar@osk.mine.nu> - 0.2.0-1
- New upstream release

* Mon Apr 23 2001 Oskar Liljeblad <osk@hem.passagen.se> - 0.1.0-1
- Initial Release
