Module['preRun'] = function () {
    FS.createPreloadedFile('/', 'content.idx', 'content.idx', true, false);
    FS.createPreloadedFile('/', 'content.pak', 'content.pak', true, false);

    // NOTE: Mount IDBFS as a disk space for savedata.
    FS.mkdir('/savedata');
    FS.mount(IDBFS, {}, '/savedata');
    console.log('Mounted /savedata');

    Module.syncDone = 0;
    FS.syncfs(true, function(err) {
        if (err) {
            console.log('Error: FS.mount and FS.syncfs failed', err);
        }
        else {
            console.log('Mounting /savedata and syncing for mount');
        }
        Module.syncDone = 1;
    });
};
