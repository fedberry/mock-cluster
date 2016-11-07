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
    $package =  l($package['basename'], $task->url, array('attributes' => array('absolute' => TRUE)) );
    $package .= '<div>'
      . '<div>' . t('Created') . ':' . format_date($task->created, 'short') . '</div>'
      . '<div>' . t('Changed') . ':' . format_date($task->changed, 'short') . '</div>'
      .'</div>';
    $row[] = array( 'data' => $package);
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
  backdrop_add_css(backdrop_get_path('module', 'mock_cluster') . '/css/mock_cluster.css');
  $status = mock_task_status_by_id($task->status);

  $output = '<dl class="status-' . $status . ' ">';
  $output .= '<dt>' . t('TID'). '</dt>';
  $output .= '<dd>' . $task->tid . '</dd>';

  $output .= '<dt>' . t('ARCH'). '</dt>';
  $output .= '<dd>' . $task->arch . '</dd>';

  $output .= '<dt>' . t('Status'). '</dt>';
  
  $output .= '<dd >' . $status . '</dd>';

  $output .= '<dt>' . t('Owner'). '</dt>';
  $output .= '<dd>' . theme('username', array('account' => user_load($task->uid))) . '</dd>';
  
  $package = pathinfo($task->url);
  
  $output .= '<dt>' . t('Source RPM'). '</dt>';
  $output .= '<dd>' . l($package['basename'], $task->url, array('attributes' => array('absolute' => TRUE)) )  . '</dd>';

  $output .= '<dt>' . t('Created'). '</dt>';
  $output .= '<dd>' . format_date($task->created) . '</dd>';

  $output .= '<dt>' . t('Last activity'). '</dt>';
  $output .= '<dd>' . format_date($task->changed) . '</dd>';

  $log_file = 'private://mock_logs/' . $task->tid . '/exec.log';
    
    
  $output .= '<dt>' . t('Exec log'). '</dt>';
  $output .= '<dd><pre>' . file_get_contents($log_file) . '</pre></dd>';
  
  $links = array();
  
  if(is_file('private://mock_logs/' . $task->tid . '/build.log')) {
    $links['build'] = array(
      'title' => t('Build.log'),
      'href' => "tasks/" . $task->tid . "/download/build",
    );
  }

  if(is_file('private://mock_logs/' . $task->tid . '/root.log')) {
    $links['root'] = array(
      'title' => t('Root.log'),
      'href' => "tasks/" . $task->tid . "/download/root",
    );
  }

  if(is_file('private://mock_logs/' . $task->tid . '/state.log')) {  
    $links['state'] = array(
      'title' => t('State.log'),
      'href' => "tasks/" . $task->tid . "/download/state",
    );
  }

  $output .= '<dt>' . t('Mock logs'). '</dt>';
  if(!empty($links)) {
    $output .= '<dd>' . theme('links',array('links' => $links, 'attributes' => array() )). '</dd>';
  } else {
    $output .= '<dd>' . t('No mock logs available.'). '</dd>';
  }

  $output .= '</dl>';
  return $output;
}

function mock_cluster_task_download($task, $file) {
  switch($file) {
    case 'build':
    case 'state':
    case 'root':
        $log_file = 'private://mock_logs/' . $task->tid . '/' . $file . '.log';
      break;
    default:
      return backdrop_access_denied();
  }
  
  if(!is_file($log_file)){
    return backdrop_not_found();
  }
  
  $headers = array(
    'Content-Type'              => 'force-download',
    'Content-Disposition'       => 'attachment; filename="' . $file . '.log"',
    'Content-Length'            => filesize($log_file),
    'Content-Transfer-Encoding' => 'binary',
    'Pragma'                    => 'no-cache',
    'Cache-Control'             => 'must-revalidate, post-check=0, pre-check=0',
    'Expires'                   => '0',
    'Accept-Ranges'             => 'bytes'
  );
  file_transfer($log_file, $headers);
  exit();
  
}
