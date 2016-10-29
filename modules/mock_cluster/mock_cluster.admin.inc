<?php
/**
 * @file
 * Admin functionality for mock_cluster.
 */

/**
 * configure menu callback.
 */
function mock_cluster_configure($form, &$form_state) {
  $config = config('mock_cluster.settings');


  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit handler for the mock_cluster_configure() form.
 */
function mock_cluster_configure_submit($form, &$form_state) {
  $config = config('mock_cluster.settings');
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}
