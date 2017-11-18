//
//  FileData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FileData__
#define __noctisgames__FileData__

struct FileData
{
    const long data_length;
    const void *data;
    const void *file_handle;
};

#endif /* defined(__noctisgames__FileData__) */
