# Copyright (c) Microsoft Corporation. All rights reserved.
#

BEGIN {
    print "#ifndef __PAL_REDEFINES_H__";
    print "#define __PAL_REDEFINES_H__";
}
{
    # Remove the CR character in case the sources are mapped from
    # a Windows share and contain CRLF line endings
    gsub(/\r/,"", $0);

    # Skip empty lines and comment lines starting with semicolon
    if (NF && !match($0, /^[:space:]*;/))
    {
        print "#define " $0 " " prefix $0;
    }
}
END {
    print "#endif";
}
