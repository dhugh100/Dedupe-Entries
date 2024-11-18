Name:           ddup
# Version is major upstream in three parts: major.minor feature and .patch
Version:        1
# Release is number of time releasted followed by dist during build tag
Release:        0.0.0%{?dist}
Summary:        A GTK+ based duplicate file finder
Group:          Utilities
License:        GPL
Source0:        ddup.tar.gz
BuildArch:      x86_64
BuildRequires:  gtk4, openssl 

%description
A GTK+ program to find and take action on duplicate files

%build
rm -rf rpmbuild/BUILD/%{name}-%{version}-build
cp -p /home/dhugh/dd/*.c /home/dhugh/rpmbuild/BUILD/%{name}-%{version}-build
cp -p /home/dhugh/dd/*.h /home/dhugh/rpmbuild/BUILD/%{name}-%{version}-build
gcc `pkg-config --cflags gtk4` -g -o ddup main.c get_folders.c load_entry_data.c traverse.c get_hash.c get_results.c show_columns.c install_property.c work_selected.c see_entry_data.c view_file.c sort_columns.c filter_columns.c work_trash.c work_options.c -lcrypto `pkg-config --libs gtk4`

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin
install ddup $RPM_BUILD_ROOT/usr/bin/%{name}

%files
/usr/bin/%{name}

%changelog
* Wed Nov 13 2024 
- initial release 
