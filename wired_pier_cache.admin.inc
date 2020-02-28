<?php

/**
 * @file
 * Page callbacks for adding, editing and hiding for ERDDAP datasets.
 */

function wired_pier_cache_admin_datasets() {
  //  get all fields
  if ($fields = wired_pier_cache_get_all_fields()) {
    //  get all datasets
    if ($datasets = db_select('wired_pier_dataset', 'ds')
    ->fields('ds', array('dsid', 'datasetID', 'displayName', 'description', 'discontinuity', 'object', 'extra_object', 'path', 'enabled'))
    ->orderBy('ds.datasetID', 'DESC')
    ->execute()
    ->fetchAllAssoc('datasetID')
    ) {
      //  overall container
      $rendered_datasets = array(
        'results' => array(
          '#type' => 'container',
          '#attributes' => array('id' => 'results'),
        ),
      );
      foreach ($fields as $fid => $field) {
        //  set up dataset container if it doesn't exist
        if (!isset($rendered_datasets[$field->datasetID])) {
          $rendered_datasets[$field->datasetID] = array(
            '#type' => 'container',
            '#attributes' => array('class' => 'dataset'),
            'meta' => array(
              '#theme' => 'table',
              '#header' => array(t('Dataset ID'), t('Display Name'), t('Description'), t('Discontinuity'), t('Extra Object Attributes'), t('Enabled')),
              '#rows' => array( wired_pier_cache_admin_get_dataset_meta($datasets[$field->datasetID]) ),
              '#attributes' => array('class' => array('meta_table')),
             ),
            'fields' => array(
              '#theme' => 'table',
              '#header' => array(t('Field ID'), t('Display Name'), t('Description'), t('Range, lower'), t('Range, upper'), t('Category'), t('Display Units'), t('Color'), t('Area'), t('Enabled'), t('Reset Overrides to Default')),
              '#rows' => array(),
              '#attributes' => array('class' => array('fields_table')),
            ),
            '#attached' => array(
              'js' => array(
                array(
                  'type' => 'file',
                  'data' => drupal_get_path('module', 'wired_pier_cache') . '/js/wired_pier_cache.admin.js',
                ),
              ),
              'css' => array(
                array(
                  'data' => drupal_get_path('module', 'wired_pier_cache') . '/css/wired_pier_cache.admin.css',
                ),
              ),
            ),
          );
        }
        //  put field into appropriate container
        $rendered_datasets[$field->datasetID]['fields']['#rows'][] = wired_pier_cache_admin_get_field_as_row($field);
      }

      return $rendered_datasets;
    }
  }
  return false;
}

function wired_pier_cache_admin_get_dataset_meta($dataset) {
  return array(
    'data' => array(
      array('data' => $dataset->datasetID, 'class' => array("datasetID")),
      array('data' => $dataset->displayName, 'class' => array("editable"), 'data-colmn' => 'displayName', 'tabindex' => 0),
      array('data' => $dataset->description, 'class' => array("editable"), 'data-tag' => 'textarea', 'data-colmn' => 'description', 'tabindex' => 0),
      array('data' => $dataset->discontinuity, 'class' => array("editable"), 'data-tag' => 'textarea', 'data-colmn' => 'discontinuity', 'tabindex' => 0),
      array('data' => $dataset->extra_object, 'class' => array("editable"), 'data-tag' => 'textarea', 'data-colmn' => 'extra_object', 'tabindex' => 0),
      array('data' => $dataset->enabled, 'class' => array("editable", "boolean"), 'data-colmn' => 'enabled'),
    ),
    'data-datasetID' => $dataset->datasetID,
    'data-dsid' => $dataset->dsid,
  );
}
function wired_pier_cache_admin_get_field_as_row($field) {

  $row = array(
    'data' => array(
      array('data' => $field->fieldID, 'class' => 'fieldID'),
      array('data' => $field->displayName, 'class' => 'editable', 'data-colmn' => 'displayName', 'tabindex' => 0),
      array('data' => $field->description, 'class' => 'editable', 'data-tag' => 'textarea', 'data-colmn' => 'description', 'tabindex' => 0),
      array('data' => $field->range_bottom, 'class' => 'editable', 'data-colmn' => 'range_bottom', 'tabindex' => 0),
      array('data' => $field->range_top, 'class' => 'editable', 'data-colmn' => 'range_top', 'tabindex' => 0),
      array('data' => $field->category, 'class' => 'editable', 'data-colmn' => 'category', 'tabindex' => 0),
      array('data' => $field->displayUnits, 'class' => 'editable', 'data-colmn' => 'displayUnits', 'tabindex' => 0),
      array('data' => $field->color, 'class' => 'editable', 'data-colmn' => 'color', 'tabindex' => 0),
      array('data' => $field->area, 'class' => 'editable boolean', 'data-colmn' => 'area'),
      array('data' => $field->enabled, 'class' => 'editable boolean', 'data-colmn' => 'enabled'),
      array('data' => "", 'class' => 'reset'),
    ),
    'data-fid' => $field->fid,
    'data-fieldID' => $field->fieldID,
    'data-datasetID' => $field->datasetID,
  );


  if (isset($field->oid)) {
    //  set data-oid
    foreach ($row['data'] as $n => $r) {
      if (isset($r['data-colmn']) && isset($field->oid[$r['data-colmn']])) {
        $row['data'][$n]['data-oid'] = $field->oid[$r['data-colmn']];
      }
    }
    //  reset default button
    $row['data'][10]['data'] = "<button class='reset'>reset</button>";
  }
   return $row;
}

function wired_pier_cache_admin_ajax() {
  //  Get vars from POST
  extract($_POST, EXTR_SKIP);
  //  This is a dataset setting - we write to the dataset table
  if (!$fid && $datasetID && $colmn && isset($value)) {
    return db_update('wired_pier_dataset')
      ->fields(array($colmn => $value))
      ->condition('datasetID', $datasetID)
      ->execute();
  }
  //  This is a field setting
  if ($fid && $colmn && isset($value)) {
    //  delete override and return default value
    if (isset($delete) && $delete && isset($oid) && $oid !== 'undefined') {
      $num_deleted = db_delete('wired_pier_dataset_field_override')
      ->condition('oid', $oid)
      ->execute();
      if ($fid > 0) {
        if ($default = db_select('wired_pier_dataset_field', 'f')
        ->fields('f', array($colmn))
        ->condition('fid', $fid)
        ->execute()
        ->fetchAssoc()) {
          return (object) array('default' => $default[$colmn]);
        }
      }
    }
    //  For columns we don't import from ERDDAP, do NOT write to the field table
    if ($colmn == 'description' || $colmn == 'color' || $colmn == 'area' || $colmn == 'enabled') {
      return db_update('wired_pier_dataset_field')
        ->fields(array(
          $colmn => $value,
          ))
        ->condition('fid', $fid)
        ->execute();
    //  All columns that are imported from ERDDAP are written as overrides
    } else {
      $record = array(
        'fid' => $fid,
        'colmn' => $colmn,
        'value' => $value,
      );
      //  Set oid if exists in order to indicate update rather than insert
      if (isset($oid) && $oid !== 'undefined') {
        return db_update('wired_pier_dataset_field_override')
          ->fields($record)
          ->condition('oid', $oid)
          ->execute();
      } else {
        $oid = db_insert('wired_pier_dataset_field_override')
          ->fields(array_keys($record))
          ->values(array_values($record))
          ->execute();
        if (isset($oid)) {
          return (object) array('oid' => $oid);
        }
      }
    }
  }
  // send back indication of TOTAL failure
  return false;
}

function wired_pier_cache_admin_ajax_callback($result) {
  /*    for debug
  $result = json_encode($result);
  print
    "<script type='text/javascript'>
      console.log($result);
    </script>";
  */
  //  is this a new field override?
  if (is_object($result) && isset($result->oid)) {
    extract($_POST, EXTR_SKIP);
    $oid = $result->oid;
    print
    "<script type='text/javascript'>
      jQuery('tr[data-fid=\"$fid\"] td[data-colmn=\"$colmn\"]').attr('data-oid', $oid);
      //  add reset button if it doesn't exist
      if (jQuery('tr[data-fid=\"$fid\"] td.reset button.reset').length === 0) {
        jQuery('<button>', {class: 'reset'})
        .click(metaEditor.rowResetListener)
        .append('reset')
        .appendTo('tr[data-fid=\"$fid\"] td.reset');
      }
    </script>";
  }
  //  handle override reset
  if (is_object($result) && !isset($result->oid) && isset($result->default)) {
    extract($_POST, EXTR_SKIP);
    $default = $result->default;
    print
    "<script type='text/javascript'>
      jQuery('tr[data-fid=\"$fid\"] td[data-colmn=\"$colmn\"]').attr('data-value', \"$default\").html(\"$default\").removeAttr('data-oid');
      //  remove button if no more overrides
      if (jQuery('tr[data-fid=\"$fid\"] td.reset').length > 0) {
        jQuery('td.reset button.reset').remove();
      }
    </script>";
  }
}
