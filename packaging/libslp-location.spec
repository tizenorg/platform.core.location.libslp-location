
Name:       libslp-location
Summary:    Location Based Service
Version:    0.3.23
Release:    2
Group:      System/Libraries
License:    TBD
Source0:    %{name}-%{version}.tar.bz2
Requires(post):  /sbin/ldconfig
Requires(post):  /usr/bin/vconftool
Requires(postun):  /sbin/ldconfig
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gconf-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(vconf)

BuildRoot:  %{_tmppath}/%{name}-%{version}-build

%description
Location Based Service Libraries


%package devel
Summary:    Location Based Service (Development files)
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Location Based Service Development Package



%prep
%setup -q -n %{name}-%{version}


%build

./autogen.sh
./configure --prefix=%{_prefix} --enable-dlog --enable-debug

# Call make instruction with smp support
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%clean
rm -rf %{buildroot}



%post 
/sbin/ldconfig
vconftool set -t int db/location/LocationEnabled "1" -g 6514 -f
vconftool set -t int db/location/NetworkEnabled "0" -g 6514  -f
vconftool set -t int db/location/AgpsEnabled "0" -g 6514 -f
vconftool set -t int db/location/SensorEnabled "0" -g 6514 -f

%postun -p /sbin/ldconfig


%files
%{_libdir}/lib*.so.*


%files devel
%{_includedir}/location/*.h
%{_libdir}/lib*.so
%{_libdir}/pkgconfig/*.pc

