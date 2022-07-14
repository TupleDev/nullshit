#include "imports.h"

auto driver_entry( ) -> NTSTATUS
{
    const auto image = utils::get_kernel_module( "dxgkrnl.sys" );

    const auto dos = reinterpret_cast< IMAGE_DOS_HEADER * > ( image.base );

    const auto nt = reinterpret_cast< IMAGE_NT_HEADERS * >( image.base + dos->e_lfanew );

    const auto export_directory = reinterpret_cast< IMAGE_EXPORT_DIRECTORY * >( image.base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress );

    const auto address = reinterpret_cast< unsigned long * >( image.base + export_directory->AddressOfFunctions );

    const auto ordinal = reinterpret_cast< unsigned short * >( image.base + export_directory->AddressOfNameOrdinals );

    for ( auto i = 0ul; i < export_directory->NumberOfNames; ++i )
    {
        unsigned char bytes[12];
        memcpy( bytes, reinterpret_cast< void * >( image.base + address[ordinal[i]] ), sizeof( bytes ) );

        if ( bytes[0] == 0x48 && bytes[1] == 0xB8 && bytes[10] == 0xFF && bytes[11] == 0xE0 &&
            ( reinterpret_cast< uintptr_t >( bytes + 2 ) < image.base || ( reinterpret_cast< uintptr_t >( bytes + 2 ) > image.base + image.size ) ) )
        {
            const auto names = reinterpret_cast< unsigned long * >( image.base + export_directory->AddressOfNames );

            dbg( "%s detected", reinterpret_cast< char * > ( image.base + names[i] ) );
        }
    }

    return STATUS_SUCCESS;
}
