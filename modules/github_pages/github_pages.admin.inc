<?php
/**
 * @file
 * Admin functionality for github_pages.
 */

/**
 * configure menu callback.
 */
function github_pages_configure($form, &$form_state) {
  $config = config('github_pages.settings');

  $form['directory'] = array(
    '#type' => 'textfield',
    '#title' => t('Directory with files'),
    '#default_value' => $config->get('directory'),
    '#description' => t('Where is root directory with files.'),
  );

  $form['markdown'] = array(
    '#type' => 'select',
    '#title' => t('Markdown'),
    '#options' => array(
      0 => t('Default'),
      1 => t('Extra'),
    ),
    '#default_value' => $config->get('markdown'),
    '#description' => t('Use default or Extra markdown syntax.'),
  );
  
  $menus = menu_load_all();
  $menu_options = array();
  $menu_options[0] = t('No menu'); 
  foreach ($menus as $menu) {
    $menu_options[$menu['menu_name']] = $menu['title'];
  }
  
  $form['menu'] = array(
    '#type' => 'select',
    '#title' => t('Menu integration'),
    '#options' => $menu_options,
    '#default_value' => $config->get('menu'),
    '#description' => t('Use directory structure to do a menu integration.'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit handler for the github_pages_configure() form.
 */
function github_pages_configure_submit($form, &$form_state) {
  $config = config('github_pages.settings');

  $config->set('directory', $form_state['values']['directory']);
  $config->set('markdown', $form_state['values']['markdown']);
  $config->set('menu', $form_state['values']['menu']);
  
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
  github_pages_path_alias_update();
  github_pages_path_alias_clean();
}
