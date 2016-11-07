<?php
/**
 * @file
 * Tasks pages provided by mock_cluster.
 */

/**
 * Menu callback for agents.
 */

function mock_cluster_tasks(){
  backdrop_add_css(backdrop_get_path('module', 'mock_cluster') . '/css/mock_cluster.css');
  
  $tasks = db_select('mock_tasks', 'mt')
    ->fields('mt')
    ->orderBy('changed', 'DESC')
    ->execute()
    ->fetchAll();

  $rows = array();
  
  foreach ($tasks as $task) {
    $row = array();

    $agent_display = ('None');
    if($task->aid > 0 ){
      $agent = mock_agent_load_by_id($task->aid);
      $agent_display = l(substr($agent->identificationID,0,6), 'agents/' . $agent->identificationID);
    }
    
    $package = pathinfo($task->url);
    
    $row[] = array( 'data' => l($task->tid, 'tasks/' . $task->tid ) );
    $row[] = array( 'data' => l($package['basename'], $task->url, array('attributes' => array('absolute' => TRUE)) ) );
    $row[] = array( 'data' => $task->arch );
    $row[] = array( 'data' => mock_task_status_by_id($task->status) );
    $row[] = array( 'data' => $agent_display );
    $row[] = array( 'data' => theme('username', array('account' => user_load($task->uid))));
    

    $rows[] = array(
      'data' => $row,
      'class' => array('status-' . mock_task_status_by_id($task->status))
    );
  }
  $header = array(t('Task ID'), t('Package'), t('ARCH'), t('Status'), t('Agent ID'), t('Owner'));

  $output['table'] = array(
    '#theme' => 'table__mock_cluster__tasks',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No tasks created yet.'),
  );
  return $output;
}

/**
 * Add task form.
 */
function mock_task_add($form, &$form_state) {
  $form['arch'] = array(
    '#type' => 'select',
    '#title' => t('Arch'),
    '#options' => array(
      'armv6l' => 'ARMv6l',
      'armv7l' => 'ARMv7l',
    ),
  );

  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#maxlength' => 253,
    '#description' => t('SRPM url to build'),
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
function mock_task_add_submit($form, &$form_state) {
  global $user;
  $record = array(
    'arch' => $form_state['values']['arch'],
    'url' => $form_state['values']['url'],
    'created' => REQUEST_TIME,
    'changed' => REQUEST_TIME,
    'status' => MOCK_TASK_PENDING,
    'uid' => $user->uid
  );
  backdrop_write_record('mock_tasks', $record);

  backdrop_set_message(t('Task !id activated!', array('!id' => $record['tid'])));
  backdrop_goto('tasks/' . $record['tid']);
}


function mock_cluster_task($task){
  $output = '<dl>';
  $output .= '<dt>' . t('TID'). '</dt>';
  $output .= '<dd>' . $task->id . '</dd>';

  $output .= '<dt>' . t('ARCH'). '</dt>';
  $output .= '<dd>' . $task->arch . '</dd>';

  $output .= '<dt>' . t('Status'). '</dt>';
  $status = mock_task_status_by_id($task->status);

  $output .= '<dd class="status-' . $status . ' ">' . $status . '</dd>';

  $output .= '<dt>' . t('Owner'). '</dt>';
  $output .= '<dd>' . theme('username', array('account' => user_load($task->uid))) . '</dd>';
  
  $package = pathinfo($task->url);
  
  $output .= '<dt>' . t('Source RPM'). '</dt>';
  $output .= '<dd>' . l($package['basename'], $task->url, array('attributes' => array('absolute' => TRUE)) )  . '</dd>';

  $output .= '<dt>' . t('Created'). '</dt>';
  $output .= '<dd>' . format_date($task->created) . '</dd>';

  $output .= '<dt>' . t('Last activity'). '</dt>';
  $output .= '<dd>' . format_date($task->access) . '</dd>';

  $output .= '</dl>';
  return $output;
}
