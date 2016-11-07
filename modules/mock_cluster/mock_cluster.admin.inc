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


function mock_cluster_arch() {
  $query = db_select('mock_arch', 'mar')->extend('AltPager');
  
  $arches = $query
    ->fields('mar')
    ->orderBy('arch', 'ASC')
    ->execute()
    ->fetchAll();

  $rows = array();
  
  foreach ($arches as $arch) {
    $row = array();
    $row[] = array( 'data' => $arch->arch );
    $row[] = array( 'data' => l('RPM', $arch->rpm_url) );
    $row[] = array( 'data' => l('SRPM', $arch->srpm_url) );
    
    $links['delete'] = array(
      'title' => t('Delete'),
      'href' => "admin/config/mock_cluster/arch/" . $arch->arch . "/delete",
    );
    $links['edit'] = array(
      'title' => t('Edit'),
      'href' => "admin/config/mock_cluster/arch/" . $arch->arch . "/edit",
    );
    $row[] = array(
      'data' => array(
        '#type' => 'operations',
        '#links' => $links,
      ),
    );

    $rows[] = $row;
  }

  $header = array(t('ARCH'), t('RPM'), t('SRPM'), t('Operations'));

  $output['pager_before'] = array(
    '#theme' => 'altpager',
  );
  $output['table'] = array(
    '#theme' => 'table__mock_cluster__arch',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No agents available yet.'),
  );
  $output['pager_after'] = array(
    '#theme' => 'altpager',
  );
  return $output;
}


/**
 * Add task form.
 */
function mock_arch_add($form, &$form_state) {
  $form['arch'] = array(
    '#type' => 'textfield',
    '#title' => t('Arch'),
  );

  $form['rpm_url'] = array(
    '#type' => 'textfield',
    '#title' => t('RPM URL'),
    '#maxlength' => 253,
    '#description' => t('RPM ROOT url'),
  );
  $form['srpm_url'] = array(
    '#type' => 'textfield',
    '#title' => t('SRPM URL'),
    '#maxlength' => 253,
    '#description' => t('SRPM ROOT url'),
  );

  $form['rpm_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('RPM DIR'),
    '#maxlength' => 253,
    '#description' => t('RPM dir to save RPM'),
  );
  $form['srpm_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('SRPM DIR'),
    '#maxlength' => 253,
    '#description' => t('SRPM dir to save SRPM'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Submit handler for the mock_task_add() form.
 */
function mock_arch_add_submit($form, &$form_state) {
  global $user;
  $record = array(
    'arch' => $form_state['values']['arch'],
    'rpm_url' => $form_state['values']['rpm_url'],
    'srpm_url' => $form_state['values']['srpm_url'],
    'rpm_dir' => $form_state['values']['rpm_dir'],
    'srpm_dir' => $form_state['values']['srpm_dir'],
  );
  backdrop_write_record('mock_arch', $record);

  backdrop_set_message(t('ARCH !arch created!', array('!arch' => $record['arch'])));
  backdrop_goto('admin/config/mock_cluster/arch');
}
