# Keep the build machine hostname out of the package metadata
%global _buildhost localhost

# Application id - matches gtk_application_new in main.c so Wayland can
# associate windows with the launcher
%global appid io.github.dhugh100.Dedupee

Name:           dedupee
Version:        1.0.0
Release:        1%{?dist}
Summary:        Find and take action on duplicate files using SHA256 hashes

License:        GPL-3.0-only
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  gtk4-devel
BuildRequires:  openssl-devel
BuildRequires:  glib2-devel
BuildRequires:  desktop-file-utils

Requires:       gtk4
Requires:       openssl-libs

%description
Dedupe Entries uses SHA256 hashes to identify and trash duplicate files.
The program supports manual or automatic selection of duplicates and provides
sort, filter, and search options. Duplicate groups are identified by SHA256
hash and files can be trashed individually or automatically in bulk.

%prep
%autosetup

%build
glib-compile-resources --generate-header logo.xml
glib-compile-resources --generate-source logo.xml
gcc %{optflags} -Wall $(pkg-config --cflags gtk4) \
    -o %{name} \
    lib.c work-auto.c about.c search.c main.c get-folders.c load-store.c \
    traverse.c get-hash.c get-results.c show-columns.c install-property.c \
    work-selected.c view-file.c sort-store.c filter-store.c work-trash.c \
    work-options.c logo.c \
    -lcrypto $(pkg-config --libs gtk4)

%install
rm -rf %{buildroot}
install -Dm755 %{name}           %{buildroot}%{_bindir}/%{name}
install -Dm644 %{appid}.png      %{buildroot}%{_datadir}/icons/hicolor/128x128/apps/%{appid}.png
install -Dm644 %{name}.desktop   %{buildroot}%{_datadir}/applications/%{appid}.desktop

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/%{appid}.desktop

%files
%license LICENSE
%doc README.md
%{_bindir}/%{name}
%{_datadir}/icons/hicolor/128x128/apps/%{appid}.png
%{_datadir}/applications/%{appid}.desktop

%changelog
* Fri Jun 12 2026 dhugh <dhugh100@users.noreply.github.com> - 1.0.0-1
- Release 1.0.0
* Fri Jun 12 2026 dhugh <dhugh100@users.noreply.github.com> - 1.0.0-1
- Release 1.0.0
