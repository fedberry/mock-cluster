<?php
/**
 * @file
 * Tasks pages provided by mock_cluster.
 */

/**
 * Menu callback for tasks.
 */
function mock_cluster_tasks(){
  backdrop_add_css(backdrop_get_path('module', 'mock_cluster') . '/css/mock_cluster.css');
  
  $header = array(
    array('data' => 'Task ID', 'field' => 'mt.tid',  'sort' => 'DESC'),
    array('data' => 'Package', 'field' => 'mt.url'),
    array('data' => 'ARCH', 'field' => 'mt.arch'),    
    array('data' => 'Status', 'field' => 'mt.status'),    
    array('data' => 'Agent ID', 'field' => 'mt.aid'),    
    array('data' => 'Owner', 'field' => 'mt.uid'),
    array('data' => 'Operations'),
  );
  
  $query = db_select('mock_tasks', 'mt')->extend('TableSort')->extend('AltPager');
  
  $tasks = $query
    ->fields('mt')
    ->orderByHeader($header)
    ->execute()
    ->fetchAll();

  $rows = array();
  
  $is_admin = user_access('administer mock tasks');
    
  foreach ($tasks as $task) {
    $row = array();

    $agent_display = ('None');
    if($task->aid > 0 ){
      if($agent = mock_agent_load_by_id($task->aid)) {
        $agent_display = l(substr($agent->identificationID,0,6), 'agents/' . $agent->identificationID);
      }
    }

    $title_arch = $task->arch;
    if($arch = mock_arch_load($task->arch)) {
      $title_arch = l($task->arch, $arch->rpm_url);
    }    
    
    
    $row[] = array( 'data' => l($task->tid, 'tasks/' . $task->tid ) );
    
    $package = pathinfo($task->url);
    
    $files = db_select('mock_tasks_files', 'mtf')
      ->fields('mtf')
      ->condition('tid', $task->tid)
      ->orderBy('url', 'ASC')
      ->execute()
      ->fetchAll();
    
    $links = array();
    $links['Origin_SRPM'] = array(
      'title' => t('Origin SRPM'),
      'attributes' => array('absolute' => TRUE),
      'href' => $task->url,
    );
    if($files){
      foreach($files as $file) {
        $file_info = pathinfo($file->url);
        $links[$file_info['basename']] = array(
          'title' => $file_info['basename'],
          'href' => $file->url,
          'attributes' => array('absolute' => TRUE)
        );
      }
    }
    
    $package =  l($package['basename'], 'tasks/' . $task->tid ) .  theme('links',array('links' => $links, 'attributes' => array() ));
    $package .= '<div class="timestamp">';
    
    if($task->status == 0) {
      $package .= t('Created: !time ago', array('!time' => format_interval((REQUEST_TIME - $task->created), 2)));
    } elseif($task->status == 1 ) {
      $package .= t('Started: !time ago', array('!time' => format_interval((REQUEST_TIME - $task->changed), 2)));
    }else {
      $package .= t('Finished: !time ago', array('!time' => format_interval((REQUEST_TIME - $task->changed), 2)));
    }
    $package .= '</div>';
    $row[] = array( 'data' => $package);
    
    $row[] = array( 'data' => $title_arch );
    $row[] = array( 'data' => mock_task_status_by_id($task->status) );
    $row[] = array( 'data' => $agent_display );
    $row[] = array( 'data' => theme('username', array('account' => user_load($task->uid))));
    
    $links = array();

    $links['view'] = array(
      'title' => t('View'),
      'href' => "tasks/" . $task->tid,
    );

    if($is_admin) {
      $links['edit'] = array(
        'title' => t('Edit'),
        'href' => "tasks/" . $task->tid . "/edit",
      );
    }
    $row[] = array(
        'data' => array(
          '#type' => 'operations',
          '#links' => $links,
        ),
      );
      
    $rows[] = array(
      'data' => $row,
      'class' => array('status-' . mock_task_status_by_id($task->status))
    );
  }
  
  $output['pager_before'] = array(
    '#theme' => 'altpager',
  );
  $output['table'] = array(
    '#theme' => 'table__mock_cluster__tasks',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No tasks created yet.'),
  );
  $output['pager_after'] = array(
    '#theme' => 'altpager',
  );
  return $output;
}

/**
 * Add task form.
 */
function mock_task_add($form, &$form_state) {
  
  $arches = db_select('mock_arch', 'mar')
    ->fields('mar')
    ->orderBy('arch', 'ASC')
    ->execute()
    ->fetchAll();
    
  $options = array();
  foreach($arches as $arch){
    $options[$arch->arch] = $arch->arch;
  }
    
  $form['arch'] = array(
    '#type' => 'select',
    '#title' => t('Arch'),
    '#options' => $options,
  );

  $form['url'] = array(
    '#type' => 'textarea',
    '#title' => t('URL'),
    '#description' => t('SRPM url to build. Use newline to separate multiple.'),
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
  
  $urls = explode("\n", $form_state['values']['url']);
  foreach($urls as $url){
    if(!empty($url)){
      $record = array(
        'arch' => $form_state['values']['arch'],
        'url' => $url,
        'created' => REQUEST_TIME,
        'changed' => REQUEST_TIME,
        'status' => MOCK_TASK_PENDING,
        'uid' => $user->uid
      );
      backdrop_write_record('mock_tasks', $record);
      backdrop_set_message(t('Task !id activated!', array('!id' => $record['tid'])));
    }
  }
  backdrop_goto('tasks');
}

/**
 * Edit arch form.
 */
function mock_task_edit($form, &$form_state, $task) {

  $form_state['task'] = $task;
  
  $arches = db_select('mock_arch', 'mar')
    ->fields('mar')
    ->orderBy('arch', 'ASC')
    ->execute()
    ->fetchAll();
    
  $options = array();
  foreach($arches as $arch){
    $options[$arch->arch] = $arch->arch;
  }
    
  $form['arch'] = array(
    '#type' => 'select',
    '#title' => t('Arch'),
    '#options' => $options,
    '#default_value' => $task->arch,
  );

  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#maxlength' => 253,
    '#description' => t('SRPM url'),
    '#default_value' => $task->url
  );
  $status_options = array(
    MOCK_TASK_PENDING => t('Pending'),
    MOCK_TASK_PROCESSING => t('Processing'), 
    MOCK_TASK_FAILED => t('Failed'), 
    MOCK_TASK_SUCCESS => t('Success'), 
  );
  $form['status'] = array(
    '#type' => 'select',
    '#title' => t('status'),
    '#options' => $status_options,
    '#default_value' => $task->status
    
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * Submit handler for the mock_task_edit() form.
 */
function mock_task_edit_submit($form, &$form_state) {
  global $user;
  $record = array(
    'tid' => $form_state['task']->tid,
    'url' => $form_state['values']['url'],
    'status' => $form_state['values']['status'],
    'changed' => REQUEST_TIME,
  );
  backdrop_write_record('mock_tasks', $record, array('tid'));

  backdrop_set_message(t('Task !tid updated!', array('!tid' => $form_state['task']->tid)));
  backdrop_goto('tasks');
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
  
  $files = db_select('mock_tasks_files', 'mtf')
    ->fields('mtf')
    ->condition('tid', $task->tid)
    ->orderBy('url', 'ASC')
    ->execute()
    ->fetchAll();
    
  $links = array();
  
  if($files){
    foreach($files as $file) {
      $file_info = pathinfo($file->url);
      $links[$file_info['basename']] = array(
        'title' => $file_info['basename'],
        'href' => $file->url,
        'attributes' => array('absolute' => TRUE)
      );
    }
  }
  
  $result_files = t('No files yet');
  if(!empty($links)){
    $result_files = theme('links',array('links' => $links, 'attributes' => array() ));
  }
    
  $output .= '<dt>' . t('Result files'). '</dt>';
  $output .= '<dd>' . $result_files . '</dd>';
    
  $output .= '<dt>' . t('Created'). '</dt>';
  $output .= '<dd>' . format_date($task->created) . '</dd>';

  $output .= '<dt>' . t('Last activity'). '</dt>';
  $output .= '<dd>' . format_date($task->changed) . '</dd>';

  $log_file = 'private://mock_logs/' . $task->tid . '/exec.log';
    
  
  $output .= '<dt>' . t('Exec log'). '</dt>';
  if(file_exists($log_file)){
    $output .= '<dd><pre>' . file_get_contents($log_file) . '</pre></dd>';
  } else {
    $output .= '<dd><pre>' . t('No log yet.') . '</pre></dd>';
  }
  
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
