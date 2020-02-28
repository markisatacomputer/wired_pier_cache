<?php
/**
 *    Implements a drush command for updating cache files
 *
 */

function wired_pier_cache_drush_command() {
  $items = array();

  $items['update-wp-cache'] = array(
    'description' => "Updates cached data files for wired pier aka environmental field station",
    'aliases' => array('wpupc'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_SITE, // No bootstrap at all.
    'arguments' => array(
      'datasetID' => 'The ID of the dataset to tune.',
    ),
  );
  $items['update-wp-current-cache'] = array(
    'description' => "Updates cached data files for wired pier current conditions page",
    'aliases' => array('wpupcc'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_SITE, // No bootstrap at all.
  );
  $items['tune-wp-cache'] = array(
    'description' => "Tunes the sample frequency of a dataset.",
    'aliases' => array('wp-tune'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_SITE, // No bootstrap at all.
    'arguments' => array(
      'datasetID' => 'The ID of the dataset to tune.',
      'frequency' => 'The maximum frequncy to throttle to, in data points per hour.  (eg. Every 5 minutes = 6 data/hr)',
    ),
    'examples' => array(
      'drush tune-wp-cache "exploreusgsdata", 10' => 'Tune the .',
    ),
  );

  return $items;
}

function drush_wired_pier_cache_update_wp_cache($datasetID = NULL) {
  if (function_exists('wired_pier_cache_update_cache')) {
    if (isset($datasetID)) {
      wired_pier_cache_update_cache($datasetID);
    } else {
      wired_pier_cache_update_cache();
    }
  }
}
function drush_wired_pier_cache_update_wp_current_cache() {
  if (function_exists('wired_pier_cache_update_current_conditions_cache')) {
    wired_pier_cache_update_current_conditions_cache();
  }
}
function drush_wired_pier_cache_tune_wp_cache($datasetID, $frequency = 10) {
  if (function_exists('wired_pier_cache_tune_sample_frequency')) {
    wired_pier_cache_tune_sample_frequency($datasetID);
  }
}