Name:       libslp-location
Summary:    Location Based Service
Version:    0.3.34
Release:    2
Group:      System/Libraries
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Requires(post):  /sbin/ldconfig
Requires(post):  /usr/bin/vconftool
Requires(postun):  /sbin/ldconfig
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gconf-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(vconf)
BuildRequires: pkgconfig(json-glib-1.0) 


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
./configure --prefix=%{_prefix} --enable-dlog 

make %{?jobs:-j%jobs}

%install
%make_install



%post 
/sbin/ldconfig
vconftool set -t int db/location/lastposition/gps/Timestamp "0" -f
vconftool set -t double db/location/lastposition/gps/Latitude "0.0" -f
vconftool set -t double db/location/lastposition/gps/Longitude "0.0" -f
vconftool set -t double db/location/lastposition/gps/Altitude "0.0" -f
vconftool set -t double db/location/lastposition/gps/HorAccuracy "0.0" -f
vconftool set -t double db/location/lastposition/gps/VerAccuracy "0.0" -f
vconftool set -t int db/location/lastposition/wps/Timestamp "0" -f
vconftool set -t double db/location/lastposition/wps/Latitude "0.0" -f
vconftool set -t double db/location/lastposition/wps/Longitude "0.0" -f
vconftool set -t double db/location/lastposition/wps/Altitude "0.0" -f
vconftool set -t double db/location/lastposition/wps/HorAccuracy "0.0" -f
vconftool set -t int db/location/lastposition/sps/Timestamp "0" -f
vconftool set -t double db/location/lastposition/sps/Latitude "0.0" -f
vconftool set -t double db/location/lastposition/sps/Longitude "0.0" -f
vconftool set -t double db/location/lastposition/sps/Altitude "0.0" -f
vconftool set -t double db/location/lastposition/sps/HorAccuracy "0.0" -f
vconftool set -t double db/location/lastposition/sps/VerAccuracy "0.0" -f
vconftool set -t int db/location/setting/GpsEnabled "0" -g 6514 -f
vconftool set -t int db/location/setting/AgpsEnabled "0" -g 6514 -f
vconftool set -t int db/location/setting/NetworkEnabled "0" -g 6514 -f
vconftool set -t int db/location/setting/SensorEnabled "0" -g 6514 -f

%postun -p /sbin/ldconfig


%files
%{_libdir}/lib*.so.*


%files devel
%{_includedir}/location/*.h
%{_libdir}/lib*.so
%{_libdir}/pkgconfig/*.pc

