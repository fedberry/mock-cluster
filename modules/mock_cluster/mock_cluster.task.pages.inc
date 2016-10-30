<?php
/**
 * @file
 * Tasks pages provided by mock_cluster.
 */

/**
 * Menu callback for agents.
 */

function mock_cluster_tasks(){
  $tasks = db_select('mock_tasks', 'mt')
    ->fields('mt')
    ->orderBy('changed', 'DESC')
    ->execute()
    ->fetchAll();

  $rows = array();
  
  foreach ($tasks as $task) {
    $row = array();
    $status = t('Pending');
    switch($agent->status) {
      case 1: 
        $status = t('Active');
        break;
      case 2: 
        $status = t('Failed');
        break;
      case 3: 
        $status = t('Success');
        break;
    }
    
    $agent_display = ('None');
    if($task->aid > 0 ){
      $agent = mock_agent_load_by_id($task->aid);
      $agent_display = l($agent->MAC, 'agents/' . $agent->MAC);
    }
    $row[] = array( 'data' => l($task->id, 'tasks/' . $agent->id ) );
    $row[] = array( 'data' => $task->arch );
    $row[] = array( 'data' => $status );
    $row[] = array( 'data' => $agent_display );
    $row[] = array( 'data' => theme('username', array('account' => user_load($task->uid))));
    

    $rows[] = $row;
  }
  $header = array(t('MAC address'), t('ARCH'), t('Status'), t('Agent MAC'), t('Owner'));

  $output['table'] = array(
    '#theme' => 'table__mock_cluster__tasks',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No tasks created yet.'),
  );
  return $output;
}
