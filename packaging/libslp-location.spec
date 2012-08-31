Name:       libslp-location
Summary:    Location Based Service
Version:    0.3.39
Release:    1
Group:      System/Libraries
License:    TBD
Source0:    %{name}-%{version}.tar.gz
Requires(post):  /sbin/ldconfig
Requires(post):  /usr/bin/vconftool
Requires(postun):  /sbin/ldconfig
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gconf-2.0)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(gmodule-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(location-appman)
#BuildRequires:  pkgconfig(json-glib-1.0)


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
vconftool set -t int db/location/last/gps/Timestamp "0" -f
vconftool set -t double db/location/last/gps/Latitude "0.0" -f
vconftool set -t double db/location/last/gps/Longitude "0.0" -f
vconftool set -t double db/location/last/gps/Altitude "0.0" -f
vconftool set -t double db/location/last/gps/HorAccuracy "0.0" -f
vconftool set -t double db/location/last/gps/VerAccuracy "0.0" -f
vconftool set -t double db/location/last/gps/Speed "0.0" -f
vconftool set -t double db/location/last/gps/Direction "0.0" -f
vconftool set -t int db/location/last/wps/Timestamp "0" -f
vconftool set -t double db/location/last/wps/Latitude "0.0" -f
vconftool set -t double db/location/last/wps/Longitude "0.0" -f
vconftool set -t double db/location/last/wps/Altitude "0.0" -f
vconftool set -t double db/location/last/wps/HorAccuracy "0.0" -f
vconftool set -t double db/location/last/wps/Speed "0.0" -f
vconftool set -t double db/location/last/wps/Direction "0.0" -f
vconftool set -t int db/location/last/cps/Timestamp "0" -f
vconftool set -t double db/location/last/cps/Latitude "0.0" -f
vconftool set -t double db/location/last/cps/Longitude "0.0" -f
vconftool set -t double db/location/last/cps/Altitude "0.0" -f
vconftool set -t double db/location/last/cps/HorAccuracy "0.0" -f
vconftool set -t int db/location/setting/GpsEnabled "0" -g 6514 -f
vconftool set -t int db/location/setting/AgpsEnabled "0" -g 6514 -f
vconftool set -t int db/location/setting/NetworkEnabled "0" -g 6514 -f
vconftool set -t int db/location/setting/SensorEnabled "0" -g 6514 -f


%postun -p /sbin/ldconfig


%files
%{_libdir}/lib*.so*


%files devel
%{_includedir}/location/*.h
%{_libdir}/pkgconfig/*.pc
