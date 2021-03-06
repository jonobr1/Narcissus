# [Narcissus](http://narcissus-app.com/)

> A light desktop application for creating visual connections of you and your actions.

> by [jonobr1](http://jonobr1.com/)

## For anything not code related

Please visit these other outlets:

+ [Home](http://narcissus-app.com/) for the public facing site.
+ [Download](http://narcissus-app.com/index.php?p=download) for the latest stable build of the software.
+ [News](http://news.narcissus-app.com/) for updates
+ [Gallery](http://narcissus-app.com/index.php?p=gallery) for videos uploaded to [vimeo](http://vimeo.com/tag:narcissus-app) or [YouTube](http://www.youtube.com/results?search_query=narcissus-app&search=tag).
+ [About](http://narcissus-app.com/index.php?p=about) for more information and inspiration on the process.

#### Please, please, please if you use the project link back to `http://narcissus-app.com/` or tag your videos with `narcissus-app` when uploading to [vimeo](http://vimeo.com/tag:narcissus-app) or [YouTube](http://www.youtube.com/results?search_query=narcissus-app&search=tag).

## In order to build the project

+ First you need to get [cinder](http://libcinder.org/) built properly. Check these tutorials out for your specific os: [Mac](http://libcinder.org/docs/welcome/MacSetup.html) | [Windows](http://libcinder.org/docs/welcome/MSWSetup.html).
+ Then add Narcissus to the mix:

The Narcissus project builds in the same hierarchy as sample projects within the `cinder` directory. You can place the project there or in some other directory at a similar sub-level as the `samples` folder.

e.g I personally have a folder in my `cinder` directory labelled `apps` where I house Narcissus. Like so:

![cinder/apps/Narcissus](https://github.com/jonobr1/Narcissus/raw/master/README/hierarchy.png "Narcissus Folder Hierarchy")

Once you've placed or more precisely pulled the project into the proper place, simply double-click the `xcode` or `vc10` project, build and run.

N.B — This has been solved with commit [86b1a90c1267fe4e8770](https://github.com/jonobr1/Narcissus/commit/86b1a90c1267fe4e8770bdb95155afbd9b37c551): ~~Currently the `vc10` project only builds debug and has not been tested. This is due to a number of link issues that I can't figure out. More info about that [here](http://news.narcissus-app.com/post/3848920979/if-you-know-your-way-around-visual-studio-c-2010) and [here](http://forum.libcinder.org/#Topic/23286000000562009). __Build at your own risk!__~~

## 

+ Why am I putting my personal project and experimentation on GitHub? [This](http://news.narcissus-app.com/post/4096586009/ive-moved-all-the-source-files-online-to-github) is a relatively cohesive answer.

Built with [cinder](http://libcinder.org/).

[Creative Commons Attribution-ShareAlike 3.0 Unported \(CC BY-SA 3.0\)](http://creativecommons.org/licenses/by-sa/3.0/)