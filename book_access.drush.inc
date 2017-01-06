<?php

function book_access_drush_command(){
    module_load_include('module', 'book_access', 'book_access');
    $grant_options = implode(',', array_map('strip_grant', BookAccess::grantIDs()));
    $items['book-access-set-role-grants'] = array(
        'description' => 'Set grants for roles on a given book.',
        'options' => array(
            'title' => array(
                'description' => 'Exact title of the book to grant.',
                'required' => 'TRUE',
            ),
            'role' => array(
                'description' => 'role names.',
                'required' => TRUE,
            ),
            'grants' => array(
                'description' => "list of grants for this role; possible values are: ($grant_options)",
                'required' => TRUE,
            ),
        ),
        'aliases' => array('ba-srg'),
    );
    return $items;
}

function strip_grant ($el) {
    return str_replace('grant_', '', $el);
}

function drush_book_access_set_role_grants(){
    $title  = drush_get_option('title');
    $role   = drush_get_option('role');
    $grants = explode(',', drush_get_option('grants'));
    
    module_load_include('module', 'book_access', 'book_access');
    $grants_allowed = array_map('strip_grant', BookAccess::grantIDs());

    $bid    = db_query_range(
        "select b.nid from node n join (select nid from book where nid = bid) b on n.nid = b.nid "
        . "where n.type = 'book' and n.title = :title;", 0, 1, array(':title' => $title))->fetchField();

    $grants = array_map(
        function ($el) use ($grants_allowed) {
            if(!in_array($el, $grants_allowed)) {
                drush_set_error("\ngrant_$el is not a valid grant.");
                exit(1);
            }
            return 'grant_'.trim($el);
            
        }, $grants);
        
    $roles_by_name = array_flip(user_roles());
    $row = new stdClass();
    $row->nid = $bid;
    $row->rid = array_key_exists($role, $roles_by_name) ? $roles_by_name[$role] : false;
    if(!$row->rid) drush_set_error("\nRole $role is not valid on this system.") && exit(1);

    $update = db_query_range(
        "SELECT 1 FROM {book_access_role} WHERE nid = :nid AND rid = :rid",
        0, 1, array(':nid' => $bid, ':rid' => $row->rid)
      )->fetchField();

    foreach (BookAccess::grantIDs() as $id) {
      $row->$id = in_array($id, $grants);
    }
    drupal_set_message(t('Before Write: RID:(%rid), Update? %up', array('%rid'=> $row->rid, '%up' => (int)$update)));

    drupal_write_record('book_access_role', $row, $update ? array('nid', 'rid') : array());
    drupal_set_message(t('Set Role grants; params: %t, %r, (%g)', array('%t'=> $title, '%r' => $role, '%g' => implode(',', $grants))));
    drupal_set_message(t('Set Role grants; found bid = %b', array('%b'=> $bid)));
}