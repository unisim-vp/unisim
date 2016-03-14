%define _disable_ld_no_undefined 1
%define major       0
%define minor       0
%define patch       0
%define release     %mkrel 1
%define version     %{major}.%{minor}.%{patch}
%define libname		%mklibname ieee1666
%define develname	%mklibname ieee1666 -d
%define staticdevelname	%mklibname ieee1666 -d -s

Summary:   A free implementation of IEEE Standard for Standard SystemC(R) Language Reference Manual (IEEE 1666-2011)
License:   BSD
Name:      libieee1666
Version:   %{version}
Release:   %{release}
Source:    %{libname}-%{version}.tar.gz
Prefix:    /usr
Group:     Development/C
URL:       http://unisim-vp.org

%description
A free implementation of IEEE Standard for Standard SystemC(R) Language Reference Manual (IEEE 1666-2011)

%package devel
Summary:	Libraries, includes, etc. to develop with SystemC
Group:		Development/C
Requires:	libieee1666 = %{version}-%{release}

%description devel
Libraries, include files, etc you can use to develop SystemC modules and simulators.

%package static-devel
Summary:	Libraries, includes, etc. to develop with SystemC
Group:		Development/C
Requires:	libieee1666 = %{version}-%{release}

%description static-devel
Libraries, include files, etc you can use to develop SystemC modules and simulators.

%prep
%autosetup

%build
%configure
make

%install
%make_install

find %{buildroot} -name "*.la" -delete

%files
%defattr(-,root,root)
%{_libdir}/*.so.%{major}.%{minor}.%{patch}
%{_libdir}/*.so.%{major}
%{_datadir}/%{name}

%files devel
%defattr(-,root,root)
%{_libdir}/*.so
%{_libdir}/pkgconfig/*
%{_includedir}/*

%files static-devel
%defattr(-,root,root)
%{_libdir}/*.a

%changelog
* Sat Mar 05 2016 Gilles Mouchard <gilles.mouchard@cea.fr> 0.0.0-1.mga5
initial release

