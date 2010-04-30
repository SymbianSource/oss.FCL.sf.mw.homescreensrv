#ifndef ACTIVITYCMD_H
#define ACTIVITYCMD_H

const TInt KRequestAppIdOffset(0);
const TInt KRequestActOffset(1);
const TInt KRequestData(2);
const TInt KRequestIdOffset(0);

const TInt KResponseDataSizeOffset(0);
const TInt KResponseIdOffset(3);
const TInt KResponseDataOffset(1);


enum ActivityCmd {
    AddActivity =0,
    UpdateActivity,
    RemoveActivity,
    RemoveApplicationActivities,
    Activities,
    ApplicationActivities,
    WaitActivity,
    LaunchActivity,
    GetThumbnail,
    GetData,
    CancelWait
};

#endif  //ACTIVITYCMD_H

