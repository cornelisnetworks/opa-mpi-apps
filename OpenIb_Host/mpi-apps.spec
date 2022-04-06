Name: opa-mpi-apps
Version: 10.11.1.3
Release: 1%{?dist}
Summary: Intel MPI benchmarks and Applications used by opa-fast-fabric
Group: System Environment/Libraries
License: GPLv2/BSD
Url: http://www.intel.com/
Source: opa-mpi-apps.tgz

AutoReq: no
Requires: atlas

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
Contains applications and source for testing MPI performance in conjunction with opa-fastfabric or stand alone.
IFSComponent: Tools_FF 10.11.1.3.1%{?dist}

%prep
%setup -q -c


%build



%install

%define mpi_apps_files bandwidth latency osu2 hpl-2.3 imb osu-micro-benchmarks-3.8-July12 mpi_multibw


mkdir -p $RPM_BUILD_ROOT/usr/src/opa/mpi_apps

cd ./MpiApps
cp ./apps/* -r $RPM_BUILD_ROOT/usr/src/opa/mpi_apps/

echo "/usr/src/opa/mpi_apps/%{mpi_apps_files}" > %{_builddir}/mpi_apps.list
sed -i 's;[ ];\n/usr/src/opa/mpi_apps/;g' %{_builddir}/mpi_apps.list 


%clean
rm -rf $RPM_BUILD_ROOT

%files -f %{_builddir}/mpi_apps.list

%changelog
* Mon Feb 26 2018 Jijun Wang <jijun.wang@intel.com> - 10.8.0.0
- Added component information in description for all rpms
* Wed Dec 2 2015 Brandon Yates <brandon.yates@intel.com>
- Initial Creation


