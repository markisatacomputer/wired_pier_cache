# Wired Pier Cache

This module was created to cache data from [Exploratorium's ERRDAP server](http://erddap.exploratorium.edu:8080/erddap/) to improve the performance of the [Data Explorer](http://www.exploratorium.edu/environmental-field-station/explore).

There are two main components of the module:

 * **Drush commands** for downloading and filtering datasets
 * **Administration page** for controlling how data layers display



### DRUSH

##### update-wp-cache (wpupc)
```
drush @alias.live wpupc
```
This command updates the dataset cache files.  Currently each dataset endpoint is queried for one year of data.  On successful response, data is saved in csv.

##### update-wp-current-cache (wpupcc)
```
drush wpupcc
```
This command updates the current conditions cache files.  Each dataset endpoint is queried for one day of data, which is then saved to json.

##### tune-wp-cache (wp-tune)
```
drush wp-tune "exploreusgsdata", 30
```
This filters dataset cache files by limiting the frequency of datapoints.  It takes two arguments:
* the unique text ID of the dataset
* the maximum number of datapoints per hour.

The simple filter algorithm removes datapoints at a frequency (eg. 3 per 10) that will bring the datasets average sample frequency equal to or below the second argument provided.  On successful completion of filter loop, cache files are overwritten.


### Admin Page
The admin page can be found at [/admin/datasets](https://www.exploratorium.edu/admin/datasets).  There you can change turn individual data layers and entire datasets on and off, overwrite their default names, display units, line colors, etc.


### Cron
There are two cron jobs set up to run each of the two drush commands that create cache files.  If you're working on wired pier project and need to edit or troubleshoot these, please contact Exploratorium Online Media Group for access details.  
