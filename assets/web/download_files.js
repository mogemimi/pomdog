// Copyright mogemimi. Distributed under the MIT license.

// Downloads files into the Emscripten file system before main() runs, and
// reports progress so that the page can show a loading screen meanwhile.
//
// Link this file with `--pre-js` ahead of the application's own pre-js file,
// then call the two functions below from that file:
//
// ```js
// var contentDownload = pomdogDownloadFiles([
//     {url: 'content.idx', path: '/content.idx'},
//     {url: 'content.pak', path: '/content.pak'},
// ]);
//
// Module['preRun'] = function () {
//     pomdogWaitForDownload(contentDownload);
// };
// ```
//
// The page receives progress through `Module.onDownloadProgress(loaded, total)`
// in bytes; `total` is 0 while the size is unknown. A failed download or file
// write aborts the runtime, which the page receives through Module.onAbort(what).

// Starts downloading `files`, an array of `{url, path}` objects where `path`
// is the absolute path to write the read-only file to in the Emscripten file system.
// Returns a handle to pass to pomdogWaitForDownload().
//
// Call it at the top level of a pre-js file, not from Module.preRun:
// Emscripten calls preRun only after the wasm module has been downloaded and
// compiled, so starting the downloads there would leave the network idle
// until then.
function pomdogDownloadFiles(files) {
    var loaded = files.map(function () { return 0; });
    // NOTE: 0 means the size is unknown, such as for a response without a
    // Content-Length header.
    var totals = files.map(function () { return 0; });

    var reportProgress = function () {
        var onProgress = Module['onDownloadProgress'];
        if (typeof onProgress !== 'function') {
            return;
        }
        var loadedSum = 0;
        var totalSum = 0;
        var totalKnown = true;
        for (var i = 0; i < files.length; i++) {
            loadedSum += loaded[i];
            totalSum += totals[i];
            totalKnown = totalKnown && totals[i] > 0;
        }
        onProgress(loadedSum, totalKnown ? totalSum : 0);
    };

    return Promise.all(files.map(function (file, index) {
        return new Promise(function (resolve, reject) {
            // NOTE: Compressed responses can report an unknown total even with
            // Content-Length. Use XHR's lengthComputable flag instead of
            // comparing progress against that header directly.
            var xhr = new XMLHttpRequest();
            xhr.open('GET', file.url, true);
            xhr.responseType = 'arraybuffer';
            xhr.onprogress = function (event) {
                loaded[index] = event.loaded;
                totals[index] = event.lengthComputable ? event.total : 0;
                reportProgress();
            };
            xhr.onload = function () {
                if (xhr.status < 200 || xhr.status >= 300) {
                    reject(new Error('failed to download ' + file.url + ': HTTP ' + xhr.status));
                    return;
                }
                if (totals[index] > 0) {
                    loaded[index] = totals[index];
                }
                else {
                    totals[index] = loaded[index];
                }
                reportProgress();
                resolve({path: file.path, data: new Uint8Array(xhr.response)});
            };
            xhr.onerror = function () {
                reject(new Error('failed to download ' + file.url));
            };
            xhr.send(null);
        });
    }));
}

// Holds back main() until `download`, a handle returned by
// pomdogDownloadFiles(), finishes and its files are written into the
// Emscripten file system. Call it from Module.preRun.
function pomdogWaitForDownload(download) {
    var dependency = 'pomdogWaitForDownload';
    addRunDependency(dependency);

    download.then(function (results) {
        results.forEach(function (result) {
            // NOTE: Hand the buffer over to the file system instead of copying it.
            try {
                FS.writeFile(result.path, result.data, {canOwn: true});
                // NOTE: Match `FS.createPreloadedFile(..., true, false)` permissions.
                FS.chmod(result.path, 0o555);
            }
            catch (error) {
                throw new Error('failed to write ' + result.path + ': ' + error.message);
            }
        });

        // NOTE: main() runs the game's initialization synchronously, which
        // keeps the page from painting until it returns. Let the browser
        // paint the final progress first; a timeout set from
        // requestAnimationFrame fires after that frame is presented. Skip
        // this wait if the page is hidden or becomes hidden before the frame.
        if (document.hidden) {
            return;
        }
        return new Promise(function (resolve) {
            var resume = function () {
                document.removeEventListener('visibilitychange', resume);
                resolve();
            };
            document.addEventListener('visibilitychange', resume);
            requestAnimationFrame(function () {
                setTimeout(resume, 0);
            });
        });
    }).then(function () {
        removeRunDependency(dependency);
    }).catch(function (error) {
        abort(error.message);
    });
}
