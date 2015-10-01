#pragma once
/*
 * Stuff. Not yet used. Not finished.
 */

/*
{
  "name": "afterhour-fortress",
  "displayName": "Afterhour Fortress",
  "user": "GenPage",
  "version": "1.0.0",
  "url": null,
  "minecraft": "1.7.10",
  "logo": {
    "url": "http:\/\/cdn.technicpack.net\/platform\/pack-logos\/451384.png?1407726501",
    "md5": "db543a51f9575ff5324aecc2dfb47036"
  },
  "background": {
    "url": "http:\/\/cdn.technicpack.net\/platform\/pack-backgrounds\/451384.png?1407726501",
    "md5": "81e8ffcc47485130a02c96d898eee099"
  },
  "solder": "http:\/\/solder.pagefortress.com\/api\/",
  "forceDir": false
}
*/
struct LegacyPackInfo
{
	QString name;
	QString displayName;
	QString user;
	QString version;
	QString url;
	QString minecraft;
	QString logo;
	QString background;
	QString solder;
	bool forceDir = false;
};