Yet another Rapidjson GitHub mirror
===================================

This is (yet another) a GitHub SVN mirror of the [Rapidjson library] [1],
a fast [JSON] [2] parser/generator for C++ with both SAX/DOM style API.

 * *Copyright*  
     (c) 2011 Milo Yip (miloyip@gmail.com, GitHub: [miloyip][0])  
     (c) 2013-2014 Philipp A. Hartmann (pah@qo.cx) (maintainer of this fork)
 * *License:* [MIT license] [3]
 * *Project website:* http://code.google.com/p/rapidjson/
 * *Documentation:* See the official project [`readme.txt`] [4] and the [user guide] [5].

In order to discuss the reintegration with the upstream sources, there is an [upstream issue][6] regarding the various patches at GitHub, as well as an [issue][7] in the [original author][0]'s (empty) rapidjson repository at GitHub.

[0]: https://github.com/miloyip
[1]: http://code.google.com/p/rapidjson/ "Rapidjson project website"
[2]: http://www.ietf.org/rfc/rfc4627.txt "JavaScript Object Notation"
[3]: http://www.opensource.org/licenses/mit-license.php "MIT license"
[4]: https://github.com/pah/rapidjson/blob/svn/trunk/readme.txt
[5]: http://code.google.com/p/rapidjson/wiki/UserGuide
[6]: http://code.google.com/p/rapidjson/issues/detail?id=106
[7]: https://github.com/miloyip/rapidjson/issues/1

Branch setup
------------

The main branches in this repository are the following:

 * `master`  
   This branch, augmented with this README, including all additional
   enhancements and fixes (superset of `for-upstream`).

 * `for-upstream`  
   A branch with patches that have been submitted upstream.

 * `svn/trunk`  
   The actual SVN mirror of the official project repository

 * `svn/authors`  
   Helper branch (detached history) to hold the SVN <-> Git author mappings.
   See the separate [README] [8].

[8]: https://github.com/pah/rapidjson/blob/svn/authors/README.md

Enhancements
------------

The more important ehancements are tracked in individual feature branches 
with accompanying discussion in a local pull-request.  These feature 
branches are based on the current `svn/trunk` branch to enable individual
merging to the upstream repostitory.

The `master` branch usually has the features merged already.
This currently includes the following features:

 * [#2 - GenericValue: add copy constructor and CopyFrom] [9]
 * [#3 - User-defined double output precision] [10]
 * [#4 - Optionally accept arbitrary root elements] [11]

[9]: https://github.com/pah/rapidjson/pull/2
[10]: https://github.com/pah/rapidjson/pull/3
[11]: https://github.com/pah/rapidjson/pull/4
