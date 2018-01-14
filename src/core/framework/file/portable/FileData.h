//
//  FileData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FileData__
#define __noctisgames__FileData__

class FileData
{
public:
    const long data_length;
    const void *data;
    const void *file_handle;
    const int flags;

	FileData(long data_length_in, void *data_in, void *file_handle_in, int flags_in = 0);
    FileData(const FileData& inFileData);
};

#endif /* defined(__noctisgames__FileData__) */
