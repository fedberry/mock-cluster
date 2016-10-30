<?php
/**
 * @file
 * Access content pages provided by mock_cluster.
 */

/**
 * Menu callback for agents.
 */

function mock_cluster_agents(){
  $agents = db_select('mock_agents', 'ma')
    ->fields('ma')
    ->orderBy('access', 'DESC')
    ->execute()
    ->fetchAll();

  $rows = array();
  foreach ($agents as $agent) {
    $row = array();
    $status = t('Pending');
    switch($agent->status) {
      case 1: 
        $status = t('Active');
        break;
      case 2: 
        $status = t('Disabled');
        break;
    }
    $row[] = array( 'data' => $agent->MAC );
    $row[] = array( 'data' => $agent->arch );
    $row[] = array( 'data' => $status );
    $rows[] = $row;
  }
  $header = array(t('MAC address'), t('ARCH'), t('status'));

  $output['table'] = array(
    '#theme' => 'table__mock_cluster__agents',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No agents available yet.'),
  );
  return $output;
}
