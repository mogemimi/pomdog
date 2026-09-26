// NOTE: Start downloading the archive now, while the wasm module is still
// loading, instead of from preRun. See assets/web/download_files.js in
// Pomdog for details.
var contentDownload = pomdogDownloadFiles([
    {url: 'content.idx', path: '/content.idx'},
    {url: 'content.pak', path: '/content.pak'},
]);

Module['preRun'] = function () {
    pomdogWaitForDownload(contentDownload);

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
