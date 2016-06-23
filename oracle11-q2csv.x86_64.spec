Summary:        Uitlity to dump oracle query results to text file
Name:           oracle11-q2csv
Version:        1.0.5
Release:        1
License:        none
Source:         %{name}.tar.gz
BuildArch:      x86_64
BuildRoot:      %{_tmppath}/%{name}-build
Group:          Productivity/Databases/Utilities
Vendor:         Alexey Vekshin
# oracle RPMs are slightly broken: no .so are mentioned in "provices" so disable autoreq
AutoReq:        no
Requires:       oracle-instantclient11.2-basic >= 11.2
Recommends:     oracle11-client-extras 

%description

Utility to dump query results to text file with optional delimitiers, transformations,
escaping etc (see readme for details)

%prep
%setup -n %{name}

%build

%pre

%install

mkdir -p $RPM_BUILD_ROOT/usr/local/bin

install -m 755 q2csv  $RPM_BUILD_ROOT/usr/local/bin

%post

%postun

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{_tmppath}/%{name}
rm -rf %{_topdir}/BUILD/%{name}

%files
%defattr(-,root,root)
/usr/local/bin/q2csv

%changelog
* Thu Jun 23 2016 alex
- initial rpm build for first public release
